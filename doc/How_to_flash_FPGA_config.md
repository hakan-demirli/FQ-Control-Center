- Change an existing config file for a Linux image:
    - Translate the .sof to .rbf with the following command:
        - ```quartus_cpf -c -o bitstream_compression=on DE10_NANO_SOC_FB.sof soc_system.rbf```
        You can also use the GUI:
        - ```files -> Convert Programming files -> ...
        Be mindful that the rbf file configurations has to be the same. If your Linux image is using a compressed config then you need a compressed config.
    - Insert the microSD Card to your host PC
        - Windows did not open the required partition on my PC and accused it by corruption. I had to thinker a little bit to fix it. But, for Linux it was directly visible. So, if you have any problems use Linux.
    - Replace the *.rbf in the microSD Card
        - *.rbf files has to have the same name. Since it is hard coded into the device tree.
    - [Optional] Create a device tree overlay
