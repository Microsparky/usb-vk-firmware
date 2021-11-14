#ifndef USB_HID_CONSUMER_H
#define USB_HID_CONSUMER_H

void usb_setup(void);
void usb_poll(void);
void usb_report(uint8_t* buf);

#endif // USB_HID_CONSUMER_H