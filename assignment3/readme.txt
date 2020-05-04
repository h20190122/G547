                                    Assignment-03
                                  (Course - EEEG547)


Summary:

In this project,the USB API is used to write and read into the flash memory of the
USB drive.A loadable block driver module is built to communicate with the USB Mass
Storage Device. The module uses SCSI commands to transfer data over the bulk
Endpoint. The read and write sector requests are conveyed through these commands.
Read and write functionalities are implemented in Bottom half of work queue.SCSI commands are sent
using wrapper function, i.e Command Block Wrapper(CBW) and the device acknowledges
the same by sending back Command Status Wrapper(CSW).




Readme:
   ● In linux,first extract VID and PID number of the USB device from the kernel
       log after connecting the USB device to the pc and running $ dmesg command
       in the terminal.
   ● Download the ‘ USB_block.c ‘ file uploaded on github which is the block
       driver code along with the ‘Makefile’ and replace the VID and PID and the
       device_size in the USB_block.c file.Build the driver using $make all
   ● Remove the inbuilt drivers for USB devices by running :
       $ sudo rmmod uas.ko
       $ sudo rmmod usb_storage.ko
   ● Insert the module to your system using
        $insmod USB_block.ko
       and verify if your disk is inserted by running
        $sudo fdisk -l
       or it can be checked using GUI.
   ● Now for mounting the disk first make a directory by
        $mkdir /media/file_name
       And then run the following mount command
        $ sudo mount -t vfat /dev /PEN_DRIVE /media/file_name
   ● To Read files from the Device use $gedit fie_name command
       To write in to files go to root user and the open using #vim file_name
       command



Status of Project:
   ● Write operation status:
       Successfully sending CBW for Write SCSI command using usb_bulk_msg function
       Successfully sent write out packet to files in Device directory
   ● Read Operation status:
       Successfully sending CBW for Read SCSI command using usb_bulk_msg function
       Successfully able to Receive Read in Block.
