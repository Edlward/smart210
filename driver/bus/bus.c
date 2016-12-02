#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");

int key_match(struct device *dev, struct device_driver *drv)		//ƥ�亯��
{
    return !strncmp(dev->kobj.name,drv->name,strlen(drv->name));	//ע��.����dev->init_name������dev->kobj.name��,dev->init_name���ÿ�
}  									//����,ʹ��dev->kobj.name�����Ƚ�


struct bus_type key_bus_type = {	//�Զ���һ��bus
	.name = "key_bus",
	.match = key_match,
	};
	
EXPORT_SYMBOL(key_bus_type);		//һ��Ҫ �������Կ��Ը�����ĺ���ʹ��

int key_bus_init(void)
{
	int ret;
	
	ret = bus_register(&key_bus_type);//������豸��,����ע��һ���Զ��������豸��linux��
	
	return ret;

}

void key_bus_exit(void)
{
	bus_unregister(&key_bus_type);	//ж����������豸
}


module_init(key_bus_init);
module_exit(key_bus_exit);