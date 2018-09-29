#!/bin/bash
if [ "$#" -ne 2 ] && [ "$#" -ne 3 ]; then
    echo "bad argument number"
    exit 1
fi

if ! [ -e $1 ];then
    echo " $1 doesn't exist, exit"
    exit 1
fi

app_dir=$1
if ! [ -e $2 ];then
    echo " $2 not found"
    exit 1
fi
update_dir=$2
#update_dir="${app_dir}/Contents.update"

relaunch=$3

cur_dir="${app_dir}/Contents"
#update_dir="${app_dir}/Contents.new"
old_dir="${app_dir}/Contents.old"

echo "app dir: ${cur_dir}"
echo "update dir: ${update_dir}"
echo "old dir: ${old_dir}"
echo "relaunch flag: ${relaunch}"

echo "update dir: ${update_dir}" 
#if [ -d ${update_dir} ]; then
#    echo "directory exists, clean up"
#    rm -rf ${update_dir} 
#    if [ $? -ne 0 ]; then
#        echo " error ${?}"; exit 1
#    fi
#fi

echo "move ${cur_dir} to ${old_dir}..."
[ -d ${old_dir} ] && rm -rf ${old_dir} 
mv ${cur_dir} ${old_dir} 
if [ $? -ne 0 ]; then
    echo "error 3"
    exit 1
fi

echo "rename ${update_dir} to ${cur_dir}..."
mv ${update_dir} ${cur_dir} 
if [ $? -ne 0 ]; then
    echo "error 4"
    exit 1
fi
rm "${app_dir}/.need_update"

echo "update success"
if [[ ${relaunch} == "yes" ]]; then
    echo "startup Redcore"
    exec "${cur_dir}/MacOS/Redcore"
fi

exit 0
