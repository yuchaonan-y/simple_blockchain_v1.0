## backup the blockchain data
## backup the sqlite datebase 


## get current date
date=$(date "+%Y-%m-%dT%H:%M:%S")
## host_server_path
DATABASEDIR=/usr/opt/block_chain/blocks.db
## backup_server_path
BACKUPDIR=/tmp/backupDB/blocks.db
if [ ! -f $BACKUPDIR ]
then
    cp $DATABASEDIR $BACKUPDIR
    exit 1
fi

## copy current database
cp $DATABASEDIR /tmp/blocks.db

maxid=`sqlite3 $BACKUPDIR "select max(id) from block"`

backblock=`sqlite3 $BACKUPDIR "select * from block where id="$maxid `
hostblock=`sqlite3 /tmp/blocks.db "select * from block where id="$maxid `

if [ "$backblock" != "$hostblock" ]
then
    echo $date "hostDB maybe has been changed!"
    echo $date "database_backup.sh :backupDB is different from hostDB" >> /tmp/ErrorGetIOF.log
    exit 1
fi

cp /tmp/blocks.db $BACKUPDIR

exit 0
