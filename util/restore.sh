#!/bin/bash

#examples(run from base git respository directory!)
# ./util/test_all.sh
# ./util/test_all.sh qemu
# ./util/test_all.sh qemu debug=yes
# ./util/test_all.sh qemu debug=yes PROG_START_FUNC=run_all

ARCH=i386

for chapter in * ; do
	if [ -d $chapter ] ; then
		cd $chapter

		for increment in * ; do
			if [ -d $increment ] ; then
				cd $increment

				echo
				echo
				echo "================================================================="
				echo "Test $chapter/$increment: starting"
				echo

				if [ -e Makefile ] ; then
					git checkout Makefile
				fi
				if [ -e i386/build.sh ] ; then
					git checkout i386/build.sh
				fi
				if [ -e i386/startup.S ] ; then
					git checkout i386/startup.S
				fi
				if [ -e arch/i386/config.ini ] ; then
					git checkout arch/i386/config.ini
				fi
				if [ -e arch/i386/interrupt.S ] ; then
					git checkout arch/i386/interrupt.S
				fi
				if [ -e arch/i386/syscall.S ] ; then
					git checkout arch/i386/syscall.S
				fi
				if [ -e arch/i386/boot/ldscript.ld ] ; then
					git checkout arch/i386/boot/ldscript.ld
				fi
				if [ -e arch/i386/boot/startup.S ] ; then
					git checkout arch/i386/boot/startup.S
				fi
				if [ -e arch/i386/boot/kernel.ld ] ; then
					git checkout arch/i386/boot/kernel.ld
				fi
				if [ -e arch/i386/boot/user.ld ] ; then
					git checkout arch/i386/boot/user.ld
				fi

				if [ -e arm/build.sh ] ; then
					git checkout arm/build.sh
				fi
				if [ -e arch/arm/config.ini ] ; then
					git checkout arch/arm/config.ini
				fi
				if [ -e arch/arm/boot/ldscript.ld ] ; then
					git checkout arch/arm/boot/ldscript.ld
				fi
				if [ -e arch/arm/boot/kernel.ld ] ; then
					git checkout arch/arm/boot/kernel.ld
				fi
				if [ -e arch/arm/boot/user.ld ] ; then
					git checkout arch/arm/boot/user.ld
				fi

				echo
				echo "Test $chapter/$increment: completed"
				echo "================================================================="
				echo
				echo
#			 sleep 1
				cd ..
			fi
		done

		cd ..
	fi
done
