#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
  .name = __stringify(KBUILD_MODNAME),
  .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
  .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
  { 0xb21dd45e, "struct_module" },
  { 0x19f030b2, "cdev_del" },
  { 0xbf16f43, "cdev_init" },
  { 0x89b301d4, "param_get_int" },
  { 0xd8e484f0, "register_chrdev_region" },
  { 0xf02c10c6, "autoremove_wake_function" },
  { 0xf6faeb02, "__kfifo_put" },
  { 0xa37e75e4, "_spin_lock" },
  { 0x7485e15e, "unregister_chrdev_region" },
  { 0x98bd6f46, "param_set_int" },
  { 0x6779a8e0, "_spin_lock_irqsave" },
  { 0xd533bec7, "__might_sleep" },
  { 0xff607f8f, "kfifo_alloc" },
  { 0x1b7d4074, "printk" },
  { 0x2da418b5, "copy_to_user" },
  { 0xcd136d00, "__kfifo_get" },
  { 0x625acc81, "__down_failed_interruptible" },
  { 0xe416ba9e, "_spin_unlock_irqrestore" },
  { 0x22f25e73, "_spin_unlock" },
  { 0x9f2a2aae, "cdev_add" },
  { 0x4292364c, "schedule" },
  { 0xcc5005fe, "msleep_interruptible" },
  { 0x7f6a6b88, "__wake_up" },
  { 0xcf3dc120, "prepare_to_wait" },
  { 0x41e653ad, "finish_wait" },
  { 0x60a4461c, "__up_wakeup" },
  { 0x96b27088, "__down_failed" },
  { 0xf2a644fb, "copy_from_user" },
  { 0x29537c9e, "alloc_chrdev_region" },
  { 0x3ad2a976, "kfifo_free" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
  "depends=";


MODULE_INFO(srcversion, "1BA4260BD1D02CCDF4C11DF");
