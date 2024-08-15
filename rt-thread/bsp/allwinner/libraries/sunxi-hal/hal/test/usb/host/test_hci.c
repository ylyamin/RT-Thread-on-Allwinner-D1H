#include <stdio.h>
//#include <platform_usb.h>
#include <hal_log.h>
#include <hal_cmd.h>
//#include <usb/hal_hci.h>


static int cmd_test_hci(int argc, const char **argv)
{
    //hal_usb_core_init();
    //hal_usb_hcd_init(0);
    //hal_usb_hcd_init(1);
    //hal_usb_manager_init();    
    //rt_usb_host_init("hwsc"); 


    //hal_hci_driverlevel_adjust(1,15);
    //hal_hci_phy_range_set(1,31);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_test_hci, cmd_test_hci, cmd_test_hci);
//INIT_DEVICE_EXPORT(cmd_test_hci);
//FINSH_FUNCTION_EXPORT_CMD(cmd_test_hci, hal_hci, hci hal APIs tests)

static int cmd_test_hci_rm(int argc, const char **argv)
{
    for (int hci_num = 0; hci_num < 2; hci_num++)
    {
        hal_usb_hcd_deinit(hci_num);
    }
    hal_usb_core_exit();

    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_test_hci_rm, cmd_test_hci_rm, cmd_test_hci_rm);
//FINSH_FUNCTION_EXPORT_CMD(cmd_test_hci_rm, hal_hci_rm, hci hal APIs tests)
