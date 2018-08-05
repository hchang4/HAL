//Virtual CAN linux driver routines

#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/kfifo.h>
#include <linux/ioport.h>
#include <linux/poll.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#include <asm/system.h>     /* cli(), *_flags */
#include <asm/uaccess.h>    /* copy_*_user */
#include <asm/atomic.h>
#include <asm/io.h>

//Local includes
#include "vcan_drv.h"
#include "vcan_enums.h"

#define DEBUG_EN
#include "debug.h"

#define MODULE_NAME "vcan_drv"

//Device open modes
#define OP_MODE_RD    0x01
#define OP_MODE_WR    0x02

//Data FIFO structure
struct vcan_fifo_data {
  struct kfifo *data_fifo;
  struct kfifo *datalen_fifo;
  spinlock_t data_fifo_lock;
  spinlock_t datalen_fifo_lock;
};

//Define this macro to enable driver statistics
#define CAN_DRV_STATS

//Device structure
struct vcan_dev {
  //Char device structure
  struct cdev cdev;

  //self identifier - unique ID for each instance of the module
  int self_id;
  unsigned int open_count;
  unsigned int open_flags;
  spinlock_t open_lock;
  struct semaphore sem;
  struct vcan_fifo_data tx_fifo;
  struct vcan_fifo_data rx_fifo;
  wait_queue_head_t rx_wait_queue;

  //Receive address 
  int rx_addr;
  //Receive address mask
  int rx_mask;
  //Receive mask enable flag
  int rx_mask_flag;

  //Flags for the TX and RX wait queues
  int rx_data_avlb_flag;

#ifdef CAN_DRV_STATS
  //Driver statistics
  unsigned long rx_packets;
  unsigned long tx_packets;
  unsigned long tx_fifo_in_bytes;
  unsigned long tx_fifo_out_bytes;
  unsigned long rx_fifo_in_bytes;
  unsigned long rx_fifo_out_bytes;
  unsigned long rx_drv_fifo_full;
#endif
};


//Functions to setup things on open and close (release)
static int vcan_dev_open_init(struct vcan_dev *dev);
static void vcan_dev_close_cleanup(struct vcan_dev *dev);

//Transmit data from the tx_fifo
static int transmit_fifo_data(struct vcan_dev *dev, struct vcan_fifo_data *fifo);

//Wrapper functions to manage the two fifos (data and data length fifos)
static int write_to_fifo(struct vcan_dev *dev, struct vcan_fifo_data *fifo, unsigned char *buf, unsigned char len);
static int read_from_fifo(struct vcan_dev *dev, struct vcan_fifo_data *fifo, unsigned char *buf, unsigned len);
static int get_fifo_status(struct vcan_fifo_data *fifo);
//Initialize a VCAN FIFO
static int init_fifo(struct vcan_fifo_data *fifo, int count, int size);
//Delete an initialized FIFO
static int delete_fifo(struct vcan_fifo_data *fifo);

//Major and minor numbers
static int vcan_major = VCAN_MAJOR;
static int vcan_minor = VCAN_MINOR;

//The user can specify the major number and the starting minor number 
//  when loading the module
module_param(vcan_major, int, S_IRUGO);
module_param(vcan_minor, int, S_IRUGO);

//The vcan device structure
static struct vcan_dev vcan_device[NR_VCAN_MODULES] = {
  {
    //Self ID should be unique!!!
    .self_id = 0,
    .open_count = 0,
    .open_flags = 0,
    .open_lock = SPIN_LOCK_UNLOCKED,
  
    //TX and RX FIFO initialization...
    .tx_fifo.data_fifo = NULL,
    .tx_fifo.datalen_fifo = NULL,
    .tx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .tx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.data_fifo = NULL,
    .rx_fifo.datalen_fifo = NULL,
    .rx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    //Flags for the TX and RX wait queues
    .rx_data_avlb_flag = 0,
  },
  {
    //Self ID should be unique!!!
    .self_id = 1,
    .open_count = 0,
    .open_flags = 0,
    .open_lock = SPIN_LOCK_UNLOCKED,
  
    //TX and RX FIFO initialization...
    .tx_fifo.data_fifo = NULL,
    .tx_fifo.datalen_fifo = NULL,
    .tx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .tx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.data_fifo = NULL,
    .rx_fifo.datalen_fifo = NULL,
    .rx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    //Flags for the TX and RX wait queues
    .rx_data_avlb_flag = 0,
  },
  {
    //Self ID should be unique!!!
    .self_id = 2,
    .open_count = 0,
    .open_flags = 0,
    .open_lock = SPIN_LOCK_UNLOCKED,
  
    //TX and RX FIFO initialization...
    .tx_fifo.data_fifo = NULL,
    .tx_fifo.datalen_fifo = NULL,
    .tx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .tx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.data_fifo = NULL,
    .rx_fifo.datalen_fifo = NULL,
    .rx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    //Flags for the TX and RX wait queues
    .rx_data_avlb_flag = 0,
  },
  {
    //Self ID should be unique!!!
    .self_id = 3,
    .open_count = 0,
    .open_flags = 0,
    .open_lock = SPIN_LOCK_UNLOCKED,
  
    //TX and RX FIFO initialization...
    .tx_fifo.data_fifo = NULL,
    .tx_fifo.datalen_fifo = NULL,
    .tx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .tx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.data_fifo = NULL,
    .rx_fifo.datalen_fifo = NULL,
    .rx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    //Flags for the TX and RX wait queues
    .rx_data_avlb_flag = 0,
  },
  {
    //Self ID should be unique!!!
    .self_id = 4,
    .open_count = 0,
    .open_flags = 0,
    .open_lock = SPIN_LOCK_UNLOCKED,
  
    //TX and RX FIFO initialization...
    .tx_fifo.data_fifo = NULL,
    .tx_fifo.datalen_fifo = NULL,
    .tx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .tx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.data_fifo = NULL,
    .rx_fifo.datalen_fifo = NULL,
    .rx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    //Flags for the TX and RX wait queues
    .rx_data_avlb_flag = 0,
  },
  {
    //Self ID should be unique!!!
    .self_id = 5, 
    .open_count = 0,
    .open_flags = 0,
    .open_lock = SPIN_LOCK_UNLOCKED,
  
    //TX and RX FIFO initialization...
    .tx_fifo.data_fifo = NULL,
    .tx_fifo.datalen_fifo = NULL,
    .tx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .tx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.data_fifo = NULL,
    .rx_fifo.datalen_fifo = NULL,
    .rx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    //Flags for the TX and RX wait queues
    .rx_data_avlb_flag = 0,
  },
  {
    //Self ID should be unique!!!
    .self_id = 6,
    .open_count = 0,
    .open_flags = 0,
    .open_lock = SPIN_LOCK_UNLOCKED,
  
    //TX and RX FIFO initialization...
    .tx_fifo.data_fifo = NULL,
    .tx_fifo.datalen_fifo = NULL,
    .tx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .tx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.data_fifo = NULL,
    .rx_fifo.datalen_fifo = NULL,
    .rx_fifo.data_fifo_lock = SPIN_LOCK_UNLOCKED,
    .rx_fifo.datalen_fifo_lock = SPIN_LOCK_UNLOCKED,
    //Flags for the TX and RX wait queues
    .rx_data_avlb_flag = 0,
  },
};


//'open' method
static int vcan_open(struct inode *inode, struct file *filep)
{
  //Device info
  struct vcan_dev *dev;
  int retval = 0;
  int minor = iminor(inode);
  int dev_ix = 0;

  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);

  //Check for a valid minor number
  if ( (minor < vcan_minor) || (minor > (vcan_minor + NR_VCAN_MODULES)) )
  {
    //Return with error if the minor number doesn't match
    return -EFAULT;
  }
  else
  {
    //Use the minor to compute the index into the device structure array
    dev_ix = minor - vcan_minor;
    DEBUG("DRV: Opened device index: %d\n", dev_ix);
  }


  spin_lock(&vcan_device[dev_ix].open_lock);
  
  if ( (vcan_device[dev_ix].open_flags & OP_MODE_RD) && (filep->f_mode & FMODE_READ))
  {
    printk(KERN_NOTICE "Device already open in READ mode.\n");
    retval = -EBUSY;
    goto open_exit;
  }
  else if (filep->f_mode & FMODE_READ)
  {
    vcan_device[dev_ix].open_flags |= OP_MODE_RD;
    //Create the RX fifo
    init_fifo(&vcan_device[dev_ix].rx_fifo, DATA_BUFFER_QUEUE_SIZE, 
              MAX_CAN_DATA_BYTES);
  }
  
  if ( (vcan_device[dev_ix].open_flags & OP_MODE_WR) && (filep->f_mode & FMODE_WRITE))
  {
    printk(KERN_NOTICE "Device already open in WRITE mode.\n");
    retval = -EBUSY;
    goto open_exit;
  }
  else if (filep->f_mode & FMODE_WRITE)
  {
    vcan_device[dev_ix].open_flags |= OP_MODE_WR;
    //Create the TX fifo ( + 2 is for storing the CAN ID (address) )
    init_fifo(&vcan_device[dev_ix].tx_fifo, DATA_BUFFER_QUEUE_SIZE, 
              (MAX_CAN_DATA_BYTES + 2) );
  }

  vcan_device[dev_ix].open_count++;

  dev = container_of(inode->i_cdev, struct vcan_dev, cdev);
  filep->private_data = dev;  //For other methods

  if (1 == vcan_device[dev_ix].open_count)
  {
    DEBUG("First open - initializing device instance\n");
    //Initialze the device and register interrupt
    if (vcan_dev_open_init(dev) < 0)
    {
      vcan_device[dev_ix].open_count = 0;
      vcan_device[dev_ix].open_flags &= ~(filep->f_flags);
      printk(KERN_ERR "Error occurred initializing the device.\n");
      retval = -EIO;
      goto open_exit;
    }
  }

open_exit:
  spin_unlock(&vcan_device[dev_ix].open_lock);
  
  return retval;
}

//'read' method
static ssize_t vcan_read(struct file *filep, char __user *buf, size_t count, 
                         loff_t *f_pos)
{
  struct vcan_dev *dev = filep->private_data;
  size_t retval;
  unsigned char rx_data[MAX_CAN_DATA_BYTES];
  int minor = iminor(filep->f_dentry->d_inode);

  //DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  //Check for a valid minor number
  if ( (minor < vcan_minor) || (minor > (vcan_minor + NR_VCAN_MODULES)) )
  {
    //Return with error if the minor number doesn't match
    return -EFAULT;
  }

  if (down_interruptible(&dev->sem))
  {
    return -ERESTARTSYS;
  }

  //Limit the max. number of bytes requested to 8
  if (count > MAX_CAN_DATA_BYTES)
  {
    count = MAX_CAN_DATA_BYTES;
  }
  //Return with error if user requests for less than 8 bytes (MAX_CAN_DATAATA_BYTES)
  else if (count < MAX_CAN_DATA_BYTES)
  {
    return -EFAULT;
  }

  //If there is no data, sleep if it's a blocking read, or return with
  //  error otherwise.
  while (!get_fifo_status(&dev->rx_fifo))
  {
    up(&dev->sem);

    //If non blocking read, return with retry later error
    if (filep->f_flags & O_NONBLOCK)
    {
      return -EBUSY;
    }
    
    //DEBUG("vcan_read: going to sleep now\n");
    //Go to sleep here
    if (wait_event_interruptible(dev->rx_wait_queue, dev->rx_data_avlb_flag != 0))
    {
      //Return on error - let the fs layer handle it
      return -ERESTARTSYS;
    }

    //DEBUG("vcan_read: woke up from sleep\n");
    
    //otherwise loop, but first reacquire the lock
    if (down_interruptible(&dev->sem))
    {
      return -ERESTARTSYS;
    }

    dev->rx_data_avlb_flag = 0;
  }
  
  //Read available data...
  retval = read_from_fifo(dev, &dev->rx_fifo, rx_data, count);
  //... and store it to user buffer
  if (copy_to_user(buf, &rx_data[0], count))
  {
    retval = -EFAULT;
    goto read_exit;
  }
  
read_exit:
  up(&dev->sem);
  
  return retval;
}

//'write' method
//First two bytes will be the CAN address, followed by the data bytes with max 8 data bytes
// On success, will return the number bytes passed to the write function and on error 
// returns -1
static ssize_t vcan_write(struct file *filep, const char __user *buf, size_t count, 
                          loff_t *f_pos)
{
  struct vcan_dev *dev = filep->private_data;
  size_t retval;
  //Two additional bytes for the address
  unsigned char tx_data[MAX_CAN_DATA_BYTES + 2];
  int minor = iminor(filep->f_dentry->d_inode);

  //DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);

  //Check for a valid minor number
  if ( (minor < vcan_minor) || (minor > (vcan_minor + NR_VCAN_MODULES)) )
  {
    //Return with error if the minor number doesn't match
    return -EFAULT;
  }

  if (down_interruptible(&dev->sem))
  {
    return -ERESTARTSYS;
  }

  //The first two bytes form the address. Check for a min. of 1 data byte
  //  and a max. of 8 data bytes
  if (count < 3)
  {
    retval = -EFAULT;
    goto write_exit;
  }
  //Can only send a max. of 8 data bytes at a time
  else if (count > (MAX_CAN_DATA_BYTES + 2))
  {
    count = MAX_CAN_DATA_BYTES + 2;
  }

  //DEBUG("DRV: vcan_write: copying data from user.\n");
  //Copy data from the user buffer
  if (copy_from_user(&tx_data[0], buf, count))
  {
    retval = -EFAULT;
    goto write_exit;
  }

  //Save data to the FIFO
  retval = write_to_fifo(dev, &dev->tx_fifo, tx_data, count);

  //Error saving to FIFO - usual cause is lack of space
  if ((int)retval < 0)
  {
    //DEBUG("DRV: Error saving data\n");
    //the 'retval' is the value returned by write_to_fifo()
    goto write_exit;
  }
  //Return number of bytes (including address)
  else
  {
    //DEBUG("DRV: Saved %d\n", count);
    retval = count;
  }

  // Check if the FIFO still contains data...
  if (get_fifo_status(&dev->tx_fifo))
  {
    //Send data in the FIFO
    if ( (int) (retval = transmit_fifo_data(dev, &dev->tx_fifo)) < 0)
    {
      //ERROR
      //retval is whatever was returned by transmit_fifo_data()
    }
    //This indicates that the fifo is empty - data has already been sent
    else if (0 == retval)
    {
      //So, return the number of bytes put into the fifo
      retval = count;
    }
    //Data sent successfully
    else
    {
      //Return the number of bytes put into the fifo
      retval = count;
#ifdef CAN_DRV_STATS    
      //Increment number of packets sent
      dev->tx_packets++;
#endif
    }
  }

write_exit:
  
  up(&dev->sem);
  
  return retval;
}

//'poll'/'select' method
static unsigned int vcan_poll(struct file *filep, poll_table *wait)
{
  struct vcan_dev *dev = filep->private_data;
  unsigned int mask = 0;

  down(&dev->sem);
  
  poll_wait(filep, &dev->rx_wait_queue,  wait);
  
  if (get_fifo_status(&dev->rx_fifo))
  {
    //readable...
    mask |= POLLIN | POLLRDNORM;
  }

  up(&dev->sem);

  return mask;
}

//'ioctl' method
static int vcan_ioctl(struct inode *inode, struct file *filep, 
                      unsigned int cmd, unsigned long arg)
{
  int err = 0;
  int retval = 0;
  int temp = 0;
  E_MSG_BOX temp_mbox;
  int minor = iminor(inode);
  struct vcan_dev *dev = filep->private_data;
  
  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  //Check for a valid minor number
  if ( (minor < vcan_minor) || (minor > (vcan_minor + NR_VCAN_MODULES)) )
  {
    //Return with error if the minor number doesn't match
    return -EFAULT;
  }
  
  //extract the type and number bitfields, and don't decode
  //wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
  if (_IOC_TYPE(cmd) != VCAN_IOC_MAGIC)
  {
    return -ENOTTY;
  }

  if (_IOC_NR(cmd) > VCAN_IOC_MAXNR)
  {
    return -ENOTTY;
  }

  //the direction is a bitmask, and VERIFY_WRITE catches R/W
  //transfers. `Type' is user-oriented, while
  //access_ok is kernel-oriented, so the concept of "read" and
  //"write" is reversed
  if (_IOC_DIR(cmd) & _IOC_READ)
  {
    err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
  }
  else if (_IOC_DIR(cmd) & _IOC_WRITE)
  {
    err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
  }
  
  if (err) 
  {
    return -EFAULT;
  }

  if (down_interruptible(&dev->sem))
  {
    return -ERESTARTSYS;
  }

  switch(cmd)
  {
    //Set the start message box for RX 
  case VCAN_IOC_S_RXMBOX1:
    retval = __get_user(temp_mbox, (E_MSG_BOX __user *)arg);
    
    //Return on error
    if (retval)
    {
      //return retval;
      goto ioctl_exit;
    }

    DEBUG("RX Start Message Box\n");
    //ioctl present for compatibility with ML9620 driver
    break;
  
    //Set the end message box for RX
  case VCAN_IOC_S_RXMBOX2:
    retval = __get_user(temp_mbox, (E_MSG_BOX __user *)arg);

    //Return on error
    if (retval)
    {
      //return retval;
      goto ioctl_exit;
    }

    DEBUG("RX End Message Box\n");
    //ioctl present for compatibility with ML9620 driver
    break;

    //Set the RX message box mask 
  case VCAN_IOC_S_RXMASK:
    retval = __get_user(temp, (int __user *)arg);

    //Return on error
    if (retval)
    {
      //return retval;
      goto ioctl_exit;
    }

    dev->rx_mask = temp;
    DEBUG("RX Mask: %d\n", dev->rx_mask);
    break;

    //Set the RX message box address
  case VCAN_IOC_S_RXADDR:
    retval = __get_user(temp, (int __user *)arg);

    if (retval)
    {
      //return retval;
      goto ioctl_exit;
    }

    dev->rx_addr = temp;
    DEBUG("RX Address: %d\n", dev->rx_addr);
    break;

    //Enable receive mask
  case VCAN_IOC_T_RX_MASK_EN:
    dev->rx_mask_flag = MBOX_EN_ADDR_MASK;

    DEBUG("RX Mask Flag: %d\n", dev->rx_mask_flag);
    break;  

    //Disable receive mask
  case VCAN_IOC_T_RX_MASK_DIS:
    dev->rx_mask_flag = MBOX_DIS_ADDR_MASK;
    
    DEBUG("RX Mask Flag: %d\n", dev->rx_mask_flag);
    break;

  case VCAN_IOC_T_RX_CONF:
    //ioctl present for compatibility with ML9620 driver
    break;  
  
    //Set the CAN baudrate
  case VCAN_IOC_S_CAN_BAUD:
    retval = __get_user(temp, (int __user *)arg);

    if (retval)
    {
      //return retval;
      goto ioctl_exit;
    }

    //No baud rate setting for the virtual driver... just a dummy ioctl for compatibility with ML9620 driver
    DEBUG("Baud scalar: %d\n", temp);

    break;

    //Put the CAN controller in INIT mode
  case VCAN_IOC_T_CAN_INIT:
    //No modes in the virtual driver... just a dummy ioctl for compatibility with ML9620 driver
    DEBUG("Device in Init mode\n");
    break;

    //Put the CAN controller in NORMAL mode
  case VCAN_IOC_T_CAN_NORM:
    //No modes in the virtual driver... just a dummy ioctl for compatibility with ML9620 driver
    DEBUG("Device in Normal mode\n");
    break;
  
    //Copy the current statistics information to the user
  case VCAN_IOC_G_CAN_STAT:
    //No statistics from the 'device'!!! - just a dummy ioctl for compatibility with MML9620 driver
    DEBUG("Device statistics\n");
    break;
  
    //Clear the message lost flag
  case VCAN_IOC_T_CLR_MSG_LOST:
    //Dummy ioctl for compatibility with ML9620 driver
    DEBUG("Clear message lost\n");
    break;
  
  default:
    retval = -ENOTTY;
    goto ioctl_exit;
    break;
  }

ioctl_exit:
  up(&dev->sem);

  return retval;
}

//'close' or 'release' method
static int vcan_release(struct inode *inode, struct file *filep)
{
  struct vcan_dev *dev = filep->private_data;
  int retval = 0;
  int timeout_count = 0;

  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  if (dev->open_count > 0)
  {
    //If the device was opened in write mode, flush the existing data
    //if ( (filep->f_flags & O_ACCMODE) == O_WRONLY)
    if  (filep->f_mode & FMODE_WRITE)
    {
      //IMPORTANT: DO NOT printk here! Else, we lose interrupts!!!
      //Check if the fifo is empty
      do
      {
        if (get_fifo_status(&dev->tx_fifo))
        {
          //Sleep for 50 ms and repeat the check - timeout after 2 seconds (40 counts)
          msleep_interruptible(50);
          timeout_count++;
        }
        else
        {
          break;
        }
      } while (timeout_count < 40);

      //If the fifo still contains data at this stage, return with error
      //if (get_fifo_status(&dev->tx_fifo))
      if (timeout_count >= 40)
      {
        retval = -1;
        goto release_exit;
      }
    }

  release_exit:
    //Reset flags
    if (filep->f_mode & FMODE_READ)
    {
      DEBUG("Closed device opened in read mode\n");
      dev->open_flags &= ~ (OP_MODE_RD);
      //Delete the RX FIFO
      delete_fifo(&dev->rx_fifo);
      DEBUG("dev->open_flags %d\n", dev->open_flags);
    }
    
    if (filep->f_mode & FMODE_WRITE)
    {
      DEBUG("Closed device opened in write mode\n");
      dev->open_flags &= ~(OP_MODE_WR);
      //Delete the TX FIFO
      delete_fifo(&dev->tx_fifo);
      DEBUG("dev->open_flags %d\n", dev->open_flags);
    }

    spin_lock(&dev->open_lock);
    dev->open_count--;
    
    //Release all resources when the open count reaches zero
    if (0 == dev->open_count)
    {
      DEBUG("Really closing the device now!\n");
      //Release interrupt, free buffer etc...
      vcan_dev_close_cleanup(dev);
    }
    
    spin_unlock(&dev->open_lock);
  }
  
  return retval;
}

//Routes data to the correct device instances
static int route_data(int tx_addr, unsigned char *data, int len, int self_id)
{
  int dev_cnt = 0;
  int data_txed = 0;

  //DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  if (NULL == data)
  {
    return -EFAULT;
  }

  for (dev_cnt = 0; dev_cnt < NR_VCAN_MODULES; dev_cnt++)
  {
    //Don't route the data back to the sender!!!
    if (self_id == vcan_device[dev_cnt].self_id)
    {
      continue;
    }

    //spin_lock(&vcan_device[dev_cnt].open_lock);

    //Proceed only if the device instance has been opened
    if ( !vcan_device[dev_cnt].open_count)
    {
      continue;
    }

    //spin_unlock(&vcan_device[dev_cnt].open_lock);

    //If RX mask is enabled, pass the tx address through the mask and proceed to 
    //  store data only if the rx address matches the masked tx address
    if (MBOX_EN_ADDR_MASK == vcan_device[dev_cnt].rx_mask_flag)
    {
//Bug fix - this logic doesn't always work right. See below for new logic.
#if 0
      //Acceptance filtering logic:
      //  AND( XOR( rx_mask, tx_addr ), AND( rx_addr , rx_mask ) )
      //  If the result of the above expression is non-zero, reject the message. If the result
      //  is zero, accept the message.
      if (  (vcan_device[dev_cnt].rx_mask ^ tx_addr) & 
            (vcan_device[dev_cnt].rx_addr & vcan_device[dev_cnt].rx_mask) )
      {
        continue;
      }
#endif
      // First check does traditional Message ID and Mask based filtering
      // Second is a kludge to get "Preamp Streaming Start" Broadcast messages from main CPU to 
      // all Preamp boards (with CAN IDs 0x10, 0x11, 0x12)
      if ( !( (vcan_device[dev_cnt].rx_mask & tx_addr) ==
              (vcan_device[dev_cnt].rx_addr & vcan_device[dev_cnt].rx_mask) ) &&

           !(tx_addr == 0x1 && 
             (vcan_device[dev_cnt].rx_addr == 0x10 || vcan_device[dev_cnt].rx_addr == 0x11 ||vcan_device[dev_cnt].rx_addr == 0x12)) 
           
         )
      {
        continue;
      }
    }

    //If the device hasn't been opened in Read (or Read+Write) mode, 
    //  do not store data for that device instance.
    if (! vcan_device[dev_cnt].open_flags & OP_MODE_RD)
    {
      continue;
    }

    //Store received data - if the FIFO is full, the new data is 
    //  not stored (i.e. it is discarded)
    if (write_to_fifo(&vcan_device[dev_cnt], &vcan_device[dev_cnt].rx_fifo, data, len) < 0)
    {
      DEBUG("RX Fifo Full\n");
#ifdef CAN_DRV_STATS
      vcan_device[dev_cnt].rx_drv_fifo_full++;
#endif
    }

#ifdef CAN_DRV_STATS
    vcan_device[dev_cnt].rx_packets++;
#endif

    //Set flag and wake up the reader
    vcan_device[dev_cnt].rx_data_avlb_flag = 1;
    wake_up_interruptible(&vcan_device[dev_cnt].rx_wait_queue);

    //Data was successfully transmitted to at least one of the registered instances
    data_txed = 1;
  }

  if (!data_txed)
  {
    return -EIO;
  }

  return 0;
}


//Transmit data from the tx_fifo
static int transmit_fifo_data(struct vcan_dev *dev, struct vcan_fifo_data *fifo)
{
  unsigned char tx_data[MAX_CAN_DATA_BYTES + 2];
  int tx_addr;
  int data_bytes = 0; //Number of CAN data bytes to tx
  int retval = 0;
  int count = 0;

  //DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  if ( (NULL == fifo) || (NULL == fifo->data_fifo) || (NULL == fifo->datalen_fifo) ) 
  {
    return -EFAULT;
  }

  if (kfifo_len(fifo->datalen_fifo) > 0)
  {
    //Read data from the TX fifo (request for the max. - will only provide what
    //   belongs to the packet)
    count = read_from_fifo(dev, fifo, tx_data, MAX_CAN_DATA_BYTES + 2);

    //This should never be less than 3 - the write function makes sure that
    //  it returns an error under this condition
    if (count < 3)
    {
      retval = -EFAULT;
      goto tx_exit;
    }

    //Extact the address...
    tx_addr = tx_data[0] << 8;
    tx_addr |= tx_data[1];

    //This is the actual number of data bytes in the packet
    data_bytes = count - 2;

    //Route the data to the appropriate channel
    retval = route_data(tx_addr, &tx_data[2], data_bytes, dev->self_id);

    if (retval < 0)
    {
      //At the end of this func, will return the error code as returned by route_data(...)
      DEBUG("TX Error\n");
    }
    else
    {
      retval = count;
    }
  }

tx_exit:
  return retval;
}

//Wrapper method to store data to the data fifo and data length
//  to the data length fifo.
static int write_to_fifo(struct vcan_dev *dev, struct vcan_fifo_data *fifo, unsigned char *buf, unsigned char len)
{
  unsigned int putcnt = 0;
  
  //DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  if ( (NULL == fifo) || (NULL == fifo->data_fifo) || (NULL == fifo->datalen_fifo) || 
       (NULL == buf) || (0 == len))
  {
    return -EFAULT;
  }

  //The queue is full - return with error
  if (kfifo_len(fifo->datalen_fifo) >= DATA_BUFFER_QUEUE_SIZE)
  {
    return -ENOBUFS;
  }

  //Store the data first...
  putcnt = kfifo_put(fifo->data_fifo, buf, len);

  //Not all data bytes were written (fifo full)
  if (putcnt < len)
  {
    printk(KERN_ERR "%s: Failed to write all the data into the FIFO\n", __FUNCTION__);
  }

  //Store the number of bytes actually written to the fifo so
  //  that the we can properly differentiate between packets of data
  len = putcnt;

#ifdef CAN_DRV_STATS
  if (fifo == &dev->tx_fifo)
  {
    dev->tx_fifo_in_bytes += len;
  }
  else 
  {
    dev->rx_fifo_in_bytes += len;
  }
#endif

  //Store the number of bytes received
  kfifo_put(fifo->datalen_fifo, &len, 1);

  return 0;
}

//Wrapper method to read the data stored in the fifo. Returns the
//  length (number of bytes) of the packet.
static int read_from_fifo(struct vcan_dev *dev, struct vcan_fifo_data *fifo, unsigned char *buf, unsigned len)
{
  unsigned int getlen = 0;
  unsigned char nrbytes = 0;
  
  //DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  if ( (NULL == fifo) || (NULL == fifo->data_fifo) || (NULL == fifo->datalen_fifo) || 
       (NULL == buf) || (0 == len))
  {
    return -EFAULT;
  }

  //check if there are entries in the fifo
  if (kfifo_len(fifo->datalen_fifo) > 0)
  {
    //Get the length of the packet first
    kfifo_get(fifo->datalen_fifo, &nrbytes, 1);

    //Then read the number of bytes in the packet
    getlen = kfifo_get(fifo->data_fifo, buf, nrbytes);

    //If it was a short read... if we ask for more than what's present
    //  (shouldn't happen in our case - we know the number of bytes
    //  to read from the fifo)
    if (getlen != nrbytes)
    {
      printk(KERN_ERR "Failed to read all data for this packet from fifo\n");
    }

#ifdef CAN_DRV_STATS
    if (fifo == &dev->tx_fifo)
    {
      dev->tx_fifo_out_bytes += getlen;
    }
    else
    {
      dev->rx_fifo_out_bytes += getlen;
    }
#endif

    //len: We ignore this because we HAVE to read all the data bytes belonging
    // to a packet & not the requested number of packets.
  }

  return getlen;
}

//Method to check if there is data in the fifo
static int get_fifo_status(struct vcan_fifo_data *fifo)
{
  return kfifo_len(fifo->datalen_fifo);
}

//Initialize a FIFO with the specified count of the specified size
static int init_fifo(struct vcan_fifo_data *fifo, int count, int size) 
{
  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  //To buffer the actual data packets & the address to which they have to be sent (+ 2)
  fifo->data_fifo = kfifo_alloc( (count * size), GFP_KERNEL, 
                                 &fifo->data_fifo_lock);

  //To store the number of bytes received per packet
  fifo->datalen_fifo = kfifo_alloc(count, GFP_KERNEL, &fifo->datalen_fifo_lock);

  if ( (NULL == fifo->data_fifo) || (NULL == fifo->datalen_fifo) )
  {
    printk(KERN_ERR "Error creating receive FIFOs\n");
    return -1;
  }

  return 0;
}

//Delete the specified fifo
static int delete_fifo(struct vcan_fifo_data *fifo)
{
  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  if (NULL == fifo)
  {
    return -1;
  }
  
  //Free up the TX data fifo
  if (fifo->data_fifo)
  {
    kfifo_free(fifo->data_fifo);
  }

  //Free up the data length fifo
  if (fifo->datalen_fifo)
  {
    kfifo_free(fifo->datalen_fifo);
  }

  return 0;
}


//The driver file operations structure
static struct file_operations vcan_fops = {
  .owner = THIS_MODULE,
  .open = vcan_open,
  .read = vcan_read,
  .write = vcan_write,
  .poll = vcan_poll,
  .ioctl = vcan_ioctl,
  .release = vcan_release,
};

//On close, puts the chip in init mode, 
//  unregisters the interrupt, frees up the fifo
static void vcan_dev_close_cleanup(struct vcan_dev *dev)
{
  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
#ifdef CAN_DRV_STATS
  printk("TX packets: %ld\n", dev->tx_packets);
  printk("RX packets: %ld\n", dev->rx_packets);
  printk("TX FIFO in: %ld\n", dev->tx_fifo_in_bytes);
  printk("TX FIFO out: %ld\n", dev->tx_fifo_out_bytes);
  printk("RX FIFO in: %ld\n", dev->rx_fifo_in_bytes);
  printk("RX FIFO out: %ld\n", dev->rx_fifo_out_bytes);
  printk("RX FIFO full: %ld\n", dev->rx_drv_fifo_full);
#endif
}
//On open, initializes the fifo, sets up the operating mode, 
//  registers the interrupt handler etc.
static int vcan_dev_open_init(struct vcan_dev *dev)
{

  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);

#ifdef CAN_DRV_STATS
  dev->rx_packets = 0;
  dev->tx_packets = 0;
  dev->tx_fifo_in_bytes = 0;
  dev->tx_fifo_out_bytes = 0;
  dev->rx_fifo_in_bytes = 0;
  dev->rx_fifo_out_bytes = 0;
  dev->rx_drv_fifo_full = 0;
#endif

  dev->rx_addr = 0x00;
  dev->rx_mask = 0x00;
  dev->rx_mask_flag = MBOX_EN_ADDR_MASK;

  return 0;
}

//Method to initialize the character device structure and 
//  store it as part of the vcan_device structure
static int vcan_cdev_init(struct vcan_dev *dev, int index)
{
  int err = 0;
  dev_t devno = MKDEV(vcan_major, vcan_minor + index);

  //DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  cdev_init(&dev->cdev, &vcan_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &vcan_fops;
  err = cdev_add(&dev->cdev, devno, 1);

  if (err)
  {
    DEBUG(KERN_ERR "Error %d adding vcan: %d", err, index);
    return -1;
  }

  return 0;
}

//Module clean-up method
static void __exit vcan_cleanup_module(void)
{
  dev_t devno = MKDEV(vcan_major, vcan_minor);
  int dev_cnt = 0;

  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  // cleanup_module is never called if registering failed
  unregister_chrdev_region(devno, NR_VCAN_MODULES);

  //Unregister the virtual CAN devices
  for (dev_cnt = 0; dev_cnt < NR_VCAN_MODULES; dev_cnt++)
  {
    cdev_del(&vcan_device[dev_cnt].cdev);
  }
}

static int __init vcan_init_module(void)
{
  int retval = 0;
  dev_t dev = 0;
  char *dev_name = MODULE_NAME;
  int dev_cnt = 0;

  DEBUG("Mod: %s, Func: %s\n", MODULE_NAME, __FUNCTION__);
  
  //If major number is specified, use it
  if (vcan_major)
  {
    dev = MKDEV(vcan_major, vcan_minor);
    retval = register_chrdev_region(dev, NR_VCAN_MODULES, dev_name);
  }
  //else, get it dynamically
  else 
  {
    retval = alloc_chrdev_region(&dev, vcan_minor, NR_VCAN_MODULES, dev_name);
    vcan_major = MAJOR(dev);
  }

  if (retval < 0) {
    printk(KERN_WARNING "vcan: can't get major %d\n", vcan_major);
    return retval;
  }

  retval = 0;

  //Initialize & register the virtual CAN devices
  for (dev_cnt = 0; dev_cnt < NR_VCAN_MODULES; dev_cnt++)
  {
    init_MUTEX(&vcan_device[dev_cnt].sem);

    if ((retval = vcan_cdev_init(&vcan_device[dev_cnt], dev_cnt)) < 0)
    {
      printk(KERN_ERR "vcan: failed to register device instance: %d\n", dev_cnt);
      goto init_fail;
    }
    else
    {
      init_waitqueue_head(&vcan_device[dev_cnt].rx_wait_queue);
    }
  }

  //If this executes, there was no failure above!
  return 0;

init_fail:
  vcan_cleanup_module();
  return retval;
}


module_init(vcan_init_module);
module_exit(vcan_cleanup_module);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Virtual CAN Driver.");
MODULE_VERSION("2.6");


