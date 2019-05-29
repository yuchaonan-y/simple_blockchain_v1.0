##1 get data from internet of things
##2 input data to block_chain
LOCALIP=192.168.1.117:8888
date=$(date "+%Y-%m-%dT%H:%M:%S")
ERROR=error
#获取物联网信息
#curl -s https://xxzx.365kaili.net/data/iot-live-data-history/yumi/num75/jl-cmcc/b94e5cb8593bf47fdbc66228387c116bd8b50b59 >/tmp/iofjson.txt
res=""
res=$(cat /tmp/iofjson.txt| grep "${ERROR}")
if [[ "$res" != "" ]]
then
	echo $date "curl -s https://xxzx.365kaili.net/data/iot-live-data-history/yumi/num75/jl-cmcc/b94e5cb8593bf47fdbc66228387c116bd8b50b59">> /tmp/ErrorGetIOF.log
        echo $date $res >> /tmp/ErrorGetIOF.log
	echo ------------------------------------------------------------------------------ >> /tmp/ErrorGetIOF.log
	exit 1	
fi
#得到物联网数据中的第一条数据
jsondata=`cat /tmp/iofjson.txt | python -c "import json,sys;jsondata=json.load(sys.stdin);print(json.dumps(jsondata['data'][0]));"|sed -e 's/ //g' -e 's/\"/\\\"/g'` 
hex=""
hex=`curl -s http://$LOCALIP/v1/chain/char2hex --data '{"str":"'$jsondata'"}'`
if [[ $hex =~ $ERROR ]]
then
	echo $date "curl -s http://"$LOCALIP"/v1/chain/char2hex --data '{\"str\":\"'$jsondata'\"}'" >> /tmp/ErrorGetIOF.log
        echo $date $hex >> /tmp/ErrorGetIOF.log
	echo ------------------------------------------------------------------------------ >> /tmp/ErrorGetIOF.log
	exit 1 
fi
#将物联网数据转码
transaction=""
transaction=`curl -s http://$LOCALIP/v1/chain/sign_trx --data '{"trx":{"time_point":"'$date'","data":'$hex',"attach":"616464"}, "key":"5JYouEPmPwaFi6LoN3TS3S2ud8sL9UggJbEbAeP3cfxHd1u4mh1"}'` 
if [[ $transaction =~ $ERROR ]]
then
	echo $date "curl -s http://$LOCALIP/v1/chain/sign_trx --data '{\"trx\":{\"time_point\":\"'$date'\",\"data\":'$hex',\"attach\":\"616464\"}, \"key\":\"5JYouEPmPwaFi6LoN3TS3S2ud8sL9UggJbEbAeP3cfxHd1u4mh1\"}'" >> /tmp/ErrorGetIOF.log
        echo $date $transaction >> /tmp/ErrorGetIOF.log
	echo ------------------------------------------------------------------------------ >> /tmp/ErrorGetIOF.log
	exit 1 
fi
#存入交易
pushresult=""
pushresult=`curl -s http://$LOCALIP/v1/chain/push_transaction --data $transaction`
if [[ $pushresult =~ $ERROR ]]
then
	echo $date "curl -s http://$LOCALIP/v1/chain/push_transaction --data " $transaction >> /tmp/ErrorGetIOF.log
        echo $date $pushresult >> /tmp/ErrorGetIOF.log
	echo ------------------------------------------------------------------------------ >> /tmp/ErrorGetIOF.log
	exit 1 
fi
#打包区块
publicresult=""
publicresult=`curl -s http://$LOCALIP/v1/chain/publish_blk` 
if [[ $publicresult =~ $ERROR ]]
then
	echo $date "curl -s http://$LOCALIP/v1/chain/publish_blk" >> /tmp/ErrorGetIOF.log
        echo $date $publicresult >> /tmp/ErrorGetIOF.log
	echo ------------------------------------------------------------------------------ >> /tmp/ErrorGetIOF.log
	exit 1 
fi

exit 0
