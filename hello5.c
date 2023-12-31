#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Nykoniuk Yuriy <Yurasik2810@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

// Структура для зберігання часу кожного "Hello, world!" та посилання на наступний та попередній запис
struct hello_entry {
    struct list_head list;  // Список для з'єднання елементів у список
    ktime_t time;           // Час, коли відбувся вивід "Hello, world!"
};

static LIST_HEAD(hello_list);  // Голова списку для початку та закінчення списку

static unsigned int print_count = 1;
module_param(print_count, uint, S_IRUGO);
MODULE_PARM_DESC(print_count, "Кількість разів для виводу 'Hello, world!' (за замовчуванням=1)");

// Ініціалізація модуля
static int __init hello_init(void)
{
    // Перевірка на правильність значення print_count
    if (print_count == 0 || (print_count > 5 && print_count < 10)) {
        pr_warn("Print count is 0 or between 5 and 10. Defaulting to 1.\n");
    }
    BUG_ON(print_count > 10);

    // Цикл для виводу "Hello, world!" та збереження часу кожного виводу у списку
    while (print_count > 0) {
        struct hello_entry *entry;

        // Виділення пам'яті для нового запису або встановлення entry в NULL, якщо print_count = 1
        if (print_count == 1) {
            entry = NULL;
        } else {
            entry = kmalloc(sizeof(*entry), GFP_KERNEL);
        }

        // Запис часу та додавання елементу до списку
        entry->time = ktime_get();
        list_add(&entry->list, &hello_list);

        // Вивід "Hello, world!"
        pr_emerg("Hello, world!\n");

        print_count--;
    }

    return 0;
}

// Функція виходу з модуля
static void __exit hello_exit(void)
{
    struct hello_entry *entry, *temp;

    // Перебір списку та вивід часу кожного запису, видалення зі списку та звільнення пам'яті
    list_for_each_entry_safe(entry, temp, &hello_list, list) {
        pr_emerg("Time: %lld ns\n", ktime_to_ns(entry->time));
        list_del(&entry->list);
        kfree(entry);
    }
}

// Вказівники на функції ініціалізації та виходу з модуля
module_init(hello_init);
module_exit(hello_exit);

