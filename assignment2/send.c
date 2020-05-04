#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include<linux/slab.h>
#define be_to_int32(buf) (((buf)[0]<<24)|((buf)[1]<<16)|((buf)[2]<<8)|(buf)[3])
#include <stdarg.h>

#define BULK_EP_OUT 0x02
#define BULK_EP_IN 0x81
static struct usb_device *device;

typedef struct  {
	uint8_t dCBWSignature[4];
	uint32_t dCBWTag;
	uint32_t dCBWDataTransferLength;
	uint8_t bmCBWFlags;
	uint8_t bCBWLUN;
	uint8_t bCBWCBLength;
	uint8_t CBWCB[16];  // actual scsi command   
}command_block_wrapper;


struct command_status_wrapper {
	uint8_t dCSWSignature[4];
	uint32_t dCSWTag;
	uint32_t dCSWDataResidue;
	uint8_t bCSWStatus;
};

static uint8_t cdb_length[256] = {
//	 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  0
	06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  1
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  2
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  3
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  4
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  5
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  6
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  7
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  8
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  9
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  A
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  B
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  C
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  D
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  E
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  F
};



static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{   


	printk(" UAS READ Capacity Driver Inserted");



	if(id->idProduct == 0x0781)
	
	{
		printk(KERN_INFO "known device\n");
	}
	else if(id->idVendor == 0x0766)
	{
		printk(KERN_INFO "known device\n");
	}
	/*int k2;
        k2= usb_control_msg(device,usb_sndctrlpipe(device, 0),0xFF,0x21,0, 0,0,0,0);                                         //control msg
		if (k2 <0) {
		printk(" reset error");
		} else   {
		printk(KERN_INFO "reset sucess");
		}*/
	
	int k1;
	uint8_t *lun=(uint8_t *)kmalloc(1*sizeof(uint8_t),GFP_KERNEL);
	
	unsigned char epAddr, epAttr;
	struct usb_endpoint_descriptor *ep_desc;
	device=container_of(interface->dev.parent,struct usb_device,dev);

	
	k1= usb_control_msg(device,usb_rcvctrlpipe(device, 0),0xFE,0xA1,0, 0,(void*)lun,sizeof(uint8_t),1000);                                         //control msg
		if (k1 <0) {
		printk(" control msg error");
		} else   {
		printk(KERN_INFO "MAX LUN = %d\n", *lun);
		}
	                                                                        
	
	printk(" %d\n", k1);
	printk("\nReading device descriptor:\n");
	device = usb_get_dev(interface_to_usbdev(interface));                                                      // geting device discriptor
	
	printk("          VID: %04X", device->descriptor.idVendor);
	printk("          PID:%04X" , device->descriptor.idProduct);
	printk(KERN_INFO "USB DEVICE CLASS : %x", interface->cur_altsetting->desc.bInterfaceClass);
	printk(KERN_INFO "USB DEVICE SUB CLASS : %x", interface->cur_altsetting->desc.bInterfaceSubClass);
	printk(KERN_INFO "USB DEVICE Protocol : %x", interface->cur_altsetting->desc.bInterfaceProtocol);
	printk(KERN_INFO "No. of Altsettings = %d\n",interface->num_altsetting);
	printk(KERN_INFO "No. of Endpoints = %d\n", interface->cur_altsetting->desc.bNumEndpoints);
			// Check if the device is USB attaced SCSI type Mass storage class
			if(((interface->cur_altsetting->desc.bInterfaceClass==0x08)
			  &(interface->cur_altsetting->desc.bInterfaceSubClass == 0x01)
			  | (interface->cur_altsetting->desc.bInterfaceSubClass == 0x06)
			  & (interface->cur_altsetting->desc.bInterfaceProtocol == 0x50))) 
			{
				printk(KERN_INFO"This is a mass storage device which supports scsi");
			}
	
	
	  

	int p;
	for(p=0;p<interface->cur_altsetting->desc.bNumEndpoints;p++)
	{
		ep_desc = &interface->cur_altsetting->endpoint[p].desc;
		epAddr = ep_desc->bEndpointAddress;
		epAttr = ep_desc->bmAttributes;
	
		if((epAttr & USB_ENDPOINT_XFERTYPE_MASK)==USB_ENDPOINT_XFER_BULK)
		{
			if(epAddr & 0x80)
				printk(KERN_INFO "EP %d is Bulk IN addr  %d\n", p, epAddr);
			else
				printk(KERN_INFO "EP %d is Bulk OUT addr %d\n", p, epAddr);
	
		}

	}





	
	command_block_wrapper *cbw = (command_block_wrapper *) kmalloc(sizeof(cbw),GFP_KERNEL);
	static uint32_t tag = 1;

	
	uint8_t cdb[16];
	int rcnt;
	int wcnt;
	
	uint8_t cdb_len;
	
	
	uint32_t expected_tag;
	uint32_t *ret_tag=&expected_tag;
	uint32_t i, max_lba, block_size;
	int device_size,x1,x2;
	int t1,t2;
  
	memset(cdb, 0, sizeof(cdb));     
	cdb[0] = 0x25;									
  
	cdb_len = cdb_length[cdb[0]];
	cbw=(command_block_wrapper*) kmalloc(sizeof(cbw),GFP_KERNEL);				
  
	
	cbw->dCBWSignature[0] = 'U';
	cbw->dCBWSignature[1] = 'S';
	cbw->dCBWSignature[2] = 'B';
	cbw->dCBWSignature[3] = 'C';
	*ret_tag = tag;
	cbw->dCBWTag = tag++;
	cbw->dCBWDataTransferLength = 0x08;
	cbw->bmCBWFlags = 0x80;
	cbw->bCBWLUN = 0;
	cbw->bCBWCBLength = cdb_len;
	memcpy(cbw->CBWCB, cdb, cdb_len);
	
	printk(KERN_INFO"Reading Capacity:\n");
	

	uint8_t *buffer=(uint8_t *)kmalloc(8*sizeof(uint8_t),GFP_KERNEL);
	for(i=0;i<8;i++)
	*(buffer+i)=0;

	
	do{
	t1=usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),(void*)cbw, 31,&wcnt,10*HZ);
	}while((t1!=0)&&(i<2));

	t2=usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN),(void*)buffer,8,&rcnt,10*HZ);
         if( t1 != 0)
	{
		printk(KERN_INFO "cbw unsuccesfull\n");
	}
	
	printk(KERN_INFO "received %d bytes\n", rcnt);

	max_lba = be_to_int32(buffer);
	block_size = be_to_int32(buffer+4);
	x1= ((int)(max_lba+1))/(1024*1024);
	x2=x1*block_size;
	device_size=x2/1024;
	printk(KERN_INFO "Max LBA: %d, Block Size: %d (%d GB)\n", max_lba, block_size, device_size);


    printk(KERN_INFO "Pen drive (%04X:%04X) plugged\n", id->idVendor,
                                id->idProduct);
    return 0;
}

static void pen_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Pen drive removed\n");
}

static struct usb_device_id pen_table[] =
{
    { USB_DEVICE(0x0781, 0x5567) },
    { USB_DEVICE(0x0766, 0x5861) },
    {} 
};
MODULE_DEVICE_TABLE (usb, pen_table);

static struct usb_driver pen_driver =
{
    .name = "pen_driver",
    .id_table = pen_table,
    .probe = pen_probe,
    .disconnect = pen_disconnect,
};

static int __init pen_init(void)
{   printk(KERN_INFO "UAS READ Capacity Driver Inserted");
    return usb_register(&pen_driver);
}

static void __exit pen_exit(void)
{
    usb_deregister(&pen_driver);
}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PRANJAL");
MODULE_DESCRIPTION("USB PenDriver");




