@rem if run in the windows script folder, change to parent
@if not exist plugin ( pushd .. ) else pushd .

rmdir /s/q STAGING
del *.ipk
mkdir STAGING
xcopy /e/y enyo STAGING
copy junkvnc_plugin STAGING
echo filemode.755=junkvnc_plugin > STAGING\package.properties
palm-package STAGING

popd
