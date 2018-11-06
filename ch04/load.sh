
#!/bin/sh
# $Id: scull_load,v 1.4 2004/11/03 06:19:49 rubini Exp $
module="scull"
device="scull"
mode="664" # emmmmmmmmmm 为什么要设置成为这一个mode 尚且不是非常的清楚

# Group: since distributions do it differently, look for wheel or use staff
# grep -q 导致数值不会写到stdout中间

# emmmmmmmmmm　获取group 到底有什么意义的啊。
if grep -q '^staff:' /etc/group; then
    group="staff"
else
    group="wheel"
fi

# invoke insmod with all arguments we got
# and use a pathname, as insmod doesn't look in . by default
insmod ./$module.ko $* || exit 1


# retrieve major number
# 由于主设备号是动态分配的，所以需要到/proc/devices 中间查找
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)



# Remove stale nodes and replace them, then give gid and perms
# Usually the script is shorter, it's scull that has several devices in it.


# 删除可能存在的
rm -f /dev/${device}[0-3]

# 创建节点
# 似乎同一个设备包含有了两个设备号
echo "--${devices}--${major}--"
echo "/dev/${device}0 c ${major} 0"
echo "-------"

mknod /dev/${device}0 c ${major} 0


# 创建link 是为了什么
ln -sf ${device}0 /dev/${device}

chgrp $group /dev/${device}[0-3] 
chmod $mode  /dev/${device}[0-3]
