#define _K_FS_C_

#include <kernel/errno.h>
#include <kernel/memory.h>
#include <types/io.h>
#include <lib/list.h>
#include <lib/string.h>
#include "fs.h"
#include "device.h"
#include "time.h"
#include "memory.h"

static kdevice_t *disk;
#define DISK_WRITE(buffer, blocks, first_block) \
k_device_send(buffer, (first_block << 16) | blocks, 0, disk);

#define DISK_READ(buffer, blocks, first_block) \
k_device_recv(buffer, (first_block << 16) | blocks, 0, disk);

static struct fs_table *ft;
static size_t ft_size;
static struct kfile_desc *last_check;
static list_t open_files;

int k_fs_init(char *disk_device, size_t bsize, size_t blocks)
{
	disk = k_device_open ( disk_device, 0 );
	assert(disk);

	//initialize disk
	ft_size = sizeof(struct fs_table) + blocks;
	ft_size = (ft_size + bsize - 1) / bsize;
	ft = kmalloc(ft_size * bsize);
	memset(ft, 0, ft_size * bsize);
	ft->file_system_type = FS_TYPE;
	strcpy(ft->partition_name, disk_device);
	ft->block_size = bsize;
	ft->blocks = blocks;
	ft->max_files = MAXFILESONDISK;

	int i;
	for (i = ft_size; i < ft->blocks; i++)
		ft->free[i] = 1;

	DISK_WRITE(ft, ft_size, 0);

	list_init(&open_files);

	last_check = NULL;

	return 0;
}

int k_fs_is_file_open(descriptor_t *desc)
{
	kobject_t *kobj;

	kobj = desc->ptr;
	if (kobj && list_find(&kobjects, &kobj->list) &&
		(kobj->flags & KTYPE_FILE) != 0)
	{
		struct kfile_desc *fd = kobj->kobject;
		if (fd && fd->id == desc->id &&
			list_find(&open_files, &fd->list))
		{
			last_check = fd;
			return 0;
		}
	}

	return -1;
}

int k_fs_open_file(char *pathname, int flags, mode_t mode, descriptor_t *desc)
{
	struct fs_node *tfd = NULL;
	char *fname = &pathname[5];

	//check for conflicting flags
	if (	((flags & O_RDONLY) && (flags & O_WRONLY)) ||
		((flags & O_RDONLY) && (flags & O_RDWR))   ||
		((flags & O_WRONLY) && (flags & O_RDWR))
	)
		return -EINVAL;

	//check if file already open
	struct kfile_desc *fd = list_get(&open_files, FIRST);
	while (fd != NULL) {
		if (strcmp(fd->tfd->node_name, fname) == 0) {
			//already open!
			//if its open for reading and O_RDONLY is set in flags
			if ((fd->flags & O_RDONLY) == (flags & O_RDONLY))
				tfd = fd->tfd;//fine, save pointer to descriptor
			else
				return -EADDRINUSE; //not fine

		}
		fd = list_get_next(&fd->list);
	}

	if (!tfd) {
		//not already open; check if such file exists in file table
		int i;
		for (i = 0; i < ft->max_files; i++) {
			if (strcmp(ft->fd[i].node_name, fname) == 0) {
				tfd = &ft->fd[i];
				break;
			}
		}
	}

	if (!tfd) {
		//file doesn't exitst
		if ((flags & (O_CREAT | O_WRONLY)) == 0)
			return -EINVAL;

		//create fs_node in fs_table
		//1. find unused descriptor
		int i;
		for (i = 0; i < ft->max_files; i++) {
			if (ft->fd[i].node_name[0] == 0) {
				tfd = &ft->fd[i];
				break;
			}
		}
		if (!tfd)
			return -ENFILE;

		//2. initialize descriptor
		strcpy(tfd->node_name, fname);
		tfd->id = i;
		timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
		tfd->tc = tfd->ta = tfd->tm = t;
		tfd->size = 0;
		tfd->blocks = 0;
	}

	//create kobject and a new struct kfile_desc
	kobject_t *kobj = kmalloc_kobject(sizeof(struct kfile_desc));
	kobj->flags = KTYPE_FILE;
	fd = kobj->kobject;
	fd->tfd = tfd;
	fd->flags = flags;
	fd->fp = 0;
	fd->id = k_new_id ();
	list_append(&open_files, fd, &fd->list);

	//fill desc
	desc->id = fd->id;
	desc->ptr = kobj;

	return 0;
}

int k_fs_close_file(descriptor_t *desc)
{
	struct kfile_desc *fd = last_check;
	kobject_t *kobj;

	kobj = desc->ptr;
	/* - already tested!
	if (!kobj || !list_find(&kobjects, &kobj->list))
		return -EINVAL;

	fd = kobj->kobject;
	if (!fd || fd->id != desc->id)
		return -EINVAL;
	*/
	if (!list_find_and_remove(&open_files, &fd->list))
		return -EINVAL;

	kfree_kobject ( kobj );

	return 0;
}

//op = 0 => write, otherwise =>read
int k_fs_read_write(descriptor_t *desc, void *buffer, size_t size, int op)
{
	//desc already checked, use "last_check";
	struct kfile_desc *fd = last_check;

	//sanity check
	if ((op && (fd->flags & O_WRONLY)) || (!op && (fd->flags & O_RDONLY)))
		return -EPERM;
		
	timespec_t current_time;
	kclock_gettime(CLOCK_REALTIME, &current_time);

	if (op) {
		//read from offset "fd->fp" to "buffer" "size" bytes

		// possible scenarios: (#=block boundary)
		// fp % block_size == 0
		// #|fp|-----------#-----------#
		//     | size |

		// fp % block_size == 0
		// #|fp|-----------#-----------#
		//     |      size     |

		// fp % block_size > 0
		// #----|fp|-------#-----------#
		//         |size|

		// fp % block_size > 0
		// #----|fp|-------#-----------#
		//         |    size    |

		//start with block x where address fd->fp is (x=fp/bsize)
		//fd->tfd->block[x] has address of block on disk
		//read that block and copy data from fd->fp to the end of block
		//DISK_READ(buf, 1, fd->tfd->block[x]);
		//which part of buf to buffer? ...
		//read next block ...
		//stop when all required data is read or end of the file is reached

		//update "ta", fp
		//return number of bytes read

		//char buf[ft->block_size];

		//size_t block = fd->fp / ft->block_size;

		//todo
		kprintf("Tu sam fd->fp cit%d\n",fd->fp);
		kprintf("Tu sam fd->tfd->size cit%d\n",fd->tfd->size);
		kprintf("Tu sam fd->tfd->block cit%d\n",fd->tfd->block[0]);
		
		size_t vel = fd->tfd->size;
		fd->fp= fd->tfd->block[0];
		fd->tfd->block[0]=0;
		char buf[ft->block_size];//buffer
		size_t todo = size;
		size_t block = fd->fp / ft->block_size;
		size_t pom=fd->fp;
		//int poc=fd->fp % ft->block_size;
		
		kprintf("Tu saaaam i ovo je blok i vel  %d %d\n",block, ft->block_size);
		while(todo > 0 && fd->fp < vel && fd->tfd->block[block] >= 0){
		kprintf("Tu samCitam %d\n",todo); //u slj liniji ide u buffer

		DISK_READ(buf, 1, fd->fp); //ovo radi samo za osnovni slucaj
		kprintf("Tu samm %s\n",buf);

		size_t kopirati = todo;
		if(todo > vel - fd->fp){
			kopirati = vel - fd->fp;
			
		}
		if(todo > ft->block_size - fd->fp % ft->block_size){
			kopirati = ft->block_size - fd->fp % ft->block_size;
			kprintf("Tu saaaam \n");

		}
		strcpy(buffer, buf);
                kprintf("Tu sam kopiram %s velicina %d todo %d vel %d\n",buffer,kopirati,todo,vel);
		buffer+=kopirati;
		fd->fp+=kopirati;
		todo-=kopirati;

		}  

//todo
		fd->tfd->block[0]=pom;
		fd->tfd->ta=current_time;
		return size - todo;
	}
	else {
		//assume there is enough space on disk

		//write ...
		//if ...->block[x] == 0 => find free block on disk
		//when fp isn't block start, read block from disk first
		//and then replace fp+ bytes ... and then write block back

		
		size_t todo = size;
		char buf[ft->block_size];
		size_t block = fd->fp / ft->block_size;
		size_t maxfilesize = ft->block_size * MAXFILEBLOCKS;

		//todo
		while(todo > 0 && fd->fp < maxfilesize){
		//kprintf("Tu sam %d\n",size);
			/*ako je blokovi_datoteke[blok_dat] == -1 {
			nađi slobodni blok i ažuriraj blokovi_datoteke
			ako nema stani i izađi iz petlje
		}*/
		while (ft->free[block]!=1) { //zasto -1?
		//ovo ne znam
		kprintf("Tu sam aa %d\n",block);
		//fd->fp++;
		block++;
	
		}
		
		DISK_READ(buf, 1, block);//dohvati blok diska u buffer
		kprintf("Tu sam %s %d\n",buf,block);
		//DISK_READ(buf, 1,block);
		size_t kopirati = todo;
		if(todo > ft->block_size - fd->fp % ft->block_size){
			kopirati = ft->block_size - fd->fp % ft->block_size;

		}
		//kprintf("Tu sam23 %s\n",buffer);
		strcpy(buf, buffer);
		//kprintf("Tu sam23 %d\n",fd->tfd->block[block]);
		buffer+=kopirati;
		fd->fp+=kopirati;
		todo-=kopirati;
		
		}
		size_t vel=fd->fp;
		kprintf("Tu sam fd->fp %d\n",fd->fp);
		kprintf("Tu sam fd->tfd->size %d\n",fd->tfd->size);
		if (vel<fd->tfd->size) {
		vel=fd->tfd->size;
		}
		fd->tfd->size=vel+block; //???
		fd->fp=fd->fp+block-size;
		kprintf("Tu sam block 0 %d\n",fd->tfd->block[0]);
		fd->tfd->block[0]=fd->fp;

		for (int i = block; i < fd->tfd->size+block; i++) {
			ft->free[i] = 0;
			kprintf("Tu sam bb %d\n",block);
		}	
		//DISK_WRITE(buf,fd->tfd->size,0);
		
		DISK_WRITE(buf,1, block);
		kprintf("Tu sam AAAAAAAAAAAAAAAAAAAAa %d\n",block);
		
		fd->tfd->ta=current_time;
		fd->tfd->tm=current_time;
		
		return size - todo;
	}

	return 0;
}


int k_fs_wipe(char *pathname)
{
	struct fs_node *tfd = NULL;
	char *fname = &pathname[5];

	//check for conflicting flags
	/*if (	((flags & O_RDONLY) && (flags & O_WRONLY)) ||
		((flags & O_RDONLY) && (flags & O_RDWR))   ||
		((flags & O_WRONLY) && (flags & O_RDWR))
	)
		return -EINVAL;*/

	//check if file already open
	struct kfile_desc *fd = list_get(&open_files, FIRST);
	while (fd != NULL) {
		if (strcmp(fd->tfd->node_name, fname) == 0) {
			//already open!
			//if its open for reading and O_RDONLY is set in flags
				tfd = fd->tfd;//fine, save pointer to descriptor


		}
		fd = list_get_next(&fd->list);
		kprintf("Tu sam aaa %d\n",fd->fp);
	}

	if (!tfd) {
		//not already open; check if such file exists in file table
		int i;
		for (i = 0; i < ft->max_files; i++) {
			if (strcmp(ft->fd[i].node_name, fname) == 0) {
				tfd = &ft->fd[i];
				break;
			}
		}
	}
	
	        kprintf("Tu sam23 %d %d\n",tfd->size,fd->fp);
	        //DISK_WRITE(ft, ft_size, 0);
	        char buf[1]="x";
	        int upisi=tfd->size - 1;
	        
	        DISK_WRITE(buf,1,upisi); 
	       // k_fs_read_write(tfd, buf, 1,0);
			//2. initialize descriptor
		/*strcpy(tfd->node_name, fname);
		tfd->id = i;
		timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
		tfd->tc = tfd->ta = tfd->tm = t;
		tfd->size = 0;
		tfd->blocks = 0;*/
                return 0;




}

