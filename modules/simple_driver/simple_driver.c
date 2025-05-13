#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>  // Para kmalloc e kfree
#include <linux/list.h>  // Para usar listas encadeadas

#define DEVICE_NAME "simple_driver"
#define CLASS_NAME  "simple_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Author Name");
MODULE_DESCRIPTION("A simple Linux char driver with linked list for storing messages.");
MODULE_VERSION("0.3");

static int majorNumber;
static struct class *charClass = NULL;
static struct device *charDevice = NULL;
static struct list_head message_list;
static int numberOpens = 0;

struct message {
    char data[256];
    struct list_head list;
};

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// Estrutura para as operações de arquivo
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

// Função de inicialização
static int __init simple_init(void) {
    printk(KERN_INFO "Simple Driver: Initializing the LKM\n");

    // Tenta registrar um número de dispositivo dinâmico
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "Simple Driver failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Simple Driver: registered correctly with major number %d\n", majorNumber);

    // Cria a classe de dispositivo
    charClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(charClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Simple Driver: failed to register device class\n");
        return PTR_ERR(charClass);
    }
    printk(KERN_INFO "Simple Driver: device class registered correctly\n");

    // Cria o dispositivo
    charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(charDevice)) {
        class_destroy(charClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Simple Driver: failed to create the device\n");
        return PTR_ERR(charDevice);
    }
    printk(KERN_INFO "Simple Driver: device class created correctly\n");

    // Inicializa a lista de mensagens
    INIT_LIST_HEAD(&message_list);
    return 0;
}

// Função de limpeza
static void __exit simple_exit(void) {
    struct message *msg, *tmp;

    // Libera todas as mensagens armazenadas
    list_for_each_entry_safe(msg, tmp, &message_list, list) {
        list_del(&msg->list);
        kfree(msg);
    }

    device_destroy(charClass, MKDEV(majorNumber, 0));
    class_unregister(charClass);
    class_destroy(charClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "Simple Driver: Goodbye from the LKM!\n");
}

// Função para abrir o dispositivo
static int dev_open(struct inode *inodep, struct file *filep) {
    numberOpens++;
    printk(KERN_INFO "Simple Driver: device has been opened %d time(s)\n", numberOpens);
    return 0;
}

// Função para ler do dispositivo
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    struct message *msg;

    if (list_empty(&message_list)) {
        printk(KERN_INFO "Simple Driver: No messages in the list\n");
        return 0;  // Nada a ler
    }

    // Pega a primeira mensagem da lista
    msg = list_first_entry(&message_list, struct message, list);

    // Copia a mensagem para o espaço do usuário
    error_count = copy_to_user(buffer, msg->data, strlen(msg->data));

    if (error_count == 0) {
        printk(KERN_INFO "Simple Driver: sent message to the user\n");
        list_del(&msg->list);  // Remove a mensagem da lista
        kfree(msg);  // Libera a memória
        return 0;
    } else {
        printk(KERN_ALERT "Simple Driver: failed to send characters to the user\n");
        return -EFAULT;
    }
}

// Função para escrever no dispositivo
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    struct message *msg;

    if (len >= sizeof(msg->data)) {
        printk(KERN_ALERT "Simple Driver: too many characters to deal with\n");
        return -EINVAL;
    }

    // Aloca memória para uma nova mensagem
    msg = kmalloc(sizeof(struct message), GFP_KERNEL);
    if (!msg) {
        printk(KERN_ALERT "Simple Driver: failed to allocate memory for the message\n");
        return -ENOMEM;
    }

    // Armazena a mensagem
    snprintf(msg->data, sizeof(msg->data), "%s", buffer);

    // Adiciona a mensagem à lista
    list_add_tail(&msg->list, &message_list);

    printk(KERN_INFO "Simple Driver: received %zu characters from the user\n", len);

    return len;
}

// Função para liberar o dispositivo
static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Simple Driver: device successfully closed\n");
    return 0;
}

module_init(simple_init);
module_exit(simple_exit);
