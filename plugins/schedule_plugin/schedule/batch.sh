##check continuity of the blockchain
basepath=$(cd `dirname $0`; pwd)

curr=`curl -s http://192.168.1.117:8888/v1/chain/check_chain |sed 's/\"//g' |grep "result:0" |wc -l`
if [ $curr -lt 1 ]
then
    echo block has changed!
    exit 1
fi

sh $basepath/database_backup.sh
if [ $? -ne 0 ]
then
    echo backup fault!
    exit 1
fi

sh $basepath/pushblock_fromIoT.sh
if [ $? -ne 0 ]
then
    echo Get data of IoT error!
    exit 1
fi

rm -f /tmp/check_chain_ret.log

exit 0