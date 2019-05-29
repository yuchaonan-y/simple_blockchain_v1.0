echo "copy scripts begin!"
echo $1

cp -r $1/schedule ./plugins/schedule_plugin
chmod -R 775 ./plugins/schedule_plugin/schedule

echo "copy scripts end!"