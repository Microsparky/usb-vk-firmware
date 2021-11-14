// C Standard libraries
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// libopencm3 STM32
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
// #include <libopencm3/stm32/st_usbfs.h>
//#include <libopencm3/stm32/common/st_usbfs_v2.h>

// USB
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>

// Includes
#include "usb_hid_consumer.h"

// #include <libopencm3/cm3/common.h>
// #include <libopencm3/stm32/rcc.h>
// #include <libopencm3/stm32/tools.h>
// #include <libopencm3/stm32/st_usbfs.h>
// #include <libopencm3/usb/usbd.h>
// #include "../usb/usb_private.h"
// #include "common/st_usbfs_core.h"

// Pointer to the USB device handle
static usbd_device *usbd_dev;

// USB Device discriptor
const struct usb_device_descriptor dev_descr = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0,		// Device: Use class information in the Interface Descriptors
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x6666,		// Development ID
	.idProduct = 0x0001,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

// USB Endpoint descriptor
const struct usb_endpoint_descriptor hid_endpoint = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x81,					// (IN) Bits 7 Direction: 0 = Out, 1 = In
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,// bmAttributes: Interrupt endpoint
	.wMaxPacketSize = 4,						// wMaxPacketSize: 4 Byte max
	.bInterval = 0x01,							// bInterval: Polling Interval (1ms)
};

// HID Report discriptor
static const uint8_t hid_report_descriptor[] = {
	0x05, 0x0c, 		// USAGE_PAGE (Consumer)
	0x09, 0x01, 		// USAGE (Consumer Control)
	0xa1, 0x01, 		// COLLECTION (Application)
	0x05, 0x0c, 		//  USAGE_PAGE (Consumer)
	0x19, 0x00, 		//    	USAGE_MINIMUM (0x000)
	0x2a, 0xff, 0x0f, 	//    	USAGE_MAXIMUM (0xfff)
	0x15, 0x00, 		// 		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x0f, 	// 		LOGICAL_MINIMUM (4095)
	0x75, 0x10, 		// 		REPORT_SIZE 16
	0x95, 0x02, 		// 		REPORT_COUNT 2
	0x81, 0x00, 		// 		INPUT (data)
	0xc0        		// END_COLLECTION
};

// HID "functional descriptor" ... used internally by libopencm3 USB driver.
static const struct {
	struct usb_hid_descriptor hid_descriptor;
	struct {
		uint8_t bReportDescriptorType;
		uint16_t wDescriptorLength;
	} __attribute__((packed)) hid_report;

} __attribute__((packed)) hid_function = {
	.hid_descriptor = {
		.bLength = sizeof(hid_function),
		.bDescriptorType = USB_DT_HID,						// Class Descriptor Type: HID Descriptor
		.bcdHID = 0x0100,
		.bCountryCode = 0,									// Country code: Not Supported
		.bNumDescriptors = 1
	},
	.hid_report = {
		.bReportDescriptorType = USB_DT_REPORT,				// Class Descriptor Type: Report Descriptor
		.wDescriptorLength = sizeof(hid_report_descriptor)
	}
};

// USB Interface discriptor
const struct usb_interface_descriptor hid_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_HID,	// CUSTOM_HID
	.bInterfaceSubClass = 0, 			// bInterfaceSubClass : 1=BOOT, 0=no boot */
	.bInterfaceProtocol = 0, 			// nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
	.iInterface = 0,					// iInterface: Index of string descriptor
	// Descriptor ends here.  The following are used internally:
	.endpoint = &hid_endpoint,
	.extra = &hid_function,
	.extralen = sizeof(hid_function),
};

// List of USB interfaces?
const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = &hid_iface,
}};

// USB Configuration descriptor
const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,				// Value to use as an argument to select this configuration
	.iConfiguration = 0,					// Index of string descriptor describing this configuration
	.bmAttributes = 0xC0,					// bmAttributes: Bus powered, No remote wake
	.bMaxPower = 0x32,						// MaxPower 100 mA: this current is used for detecting Vbus (2mA steps)
	// Descriptor ends here.  The following are used internally:
	.interface = ifaces,
};

// USB strings
static const char *usb_strings[] = {
	"Peter Keogh",							// iManufacturer
	"USB-VK",								// iProduct
	"0001",									// iSerialNumber
};

// Buffer to be used for control requests.
uint8_t usbd_control_buffer[128];

// Handle control requests
static enum usbd_request_return_codes hid_control_request(  usbd_device *dev,  
                                                            struct usb_setup_data *req, 
                                                            uint8_t **buf, 
                                                            uint16_t *len,
			                                                void (**complete)(usbd_device *, struct usb_setup_data *))
{
	(void)complete;
	(void)dev;

	if((req->bmRequestType != 0x81) ||
	   (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
	   (req->wValue != 0x2200))
		return USBD_REQ_NOTSUPP;

	/* Handle the HID report descriptor. */
	*buf = (uint8_t *)hid_report_descriptor;
	*len = sizeof(hid_report_descriptor);

	return USBD_REQ_HANDLED;
}

// Set device configuration
static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
	(void)wValue;
	(void)dev;

	usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 4, NULL);

	usbd_register_control_callback( dev,
				                    USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
				                    USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				                    hid_control_request);
}

// // Call to initialise 
// usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev_descr, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
// usbd_register_set_config_callback(usbd_dev, hid_set_config);
//
// // Call in loop
// usbd_poll(usbd_dev);
// 
// // Call to write packet
// usbd_ep_write_packet(usbd_dev, 0x81, buf, 4);

void usb_setup(void)
{
	// Enable GPIOA clocks
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_OTGFS);

	// Set up GPIO USB pins
	// PA9	USB_VBUS
	// PA11	USB_DM
	// PA12	USB_DP
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO9);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,  GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO11 | GPIO12);

	// Initialise the USB device 
	// Parameters
	// 		driver					Appears to be a structure of function pointers the "USB device driver"
	// 		dev						Pointer to USB device descriptor. This must not be changed while the device is in use.
	// 		conf					Pointer to array of USB configuration descriptors. These must not be changed while the device is in use. The length of this array is determined by the bNumConfigurations field in the device descriptor.
	//		strings					Pointer to an array of strings for USB string descriptors. Referenced in iSomething fields, e.g. iManufacturer. Since a zero index means "no string", an iSomething value of 1 refers strings[0].
	// 		num_strings				Number of items in strings array.
	// 		control_buffer			Pointer to array that would hold the data received during control requests with DATA stage
	// 		control_buffer_size		Size of control_buffer
	usbd_dev = usbd_init(	&otgfs_usb_driver, &dev_descr, &config, 
							usb_strings, 3, usbd_control_buffer, 
							sizeof(usbd_control_buffer));

	usbd_register_set_config_callback(usbd_dev, hid_set_config);

}

void usb_poll(void){
	usbd_poll(usbd_dev);
}

void usb_report(uint8_t* buf){
	usbd_ep_write_packet(usbd_dev, 0x81, buf, 4);
}
