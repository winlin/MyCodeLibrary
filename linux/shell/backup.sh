#!/bin/sh

echo "Start Backup ..."
GIT_SOURCEDIR=~/gitsource/
BACKUP_DIR=$GIT_SOURCEDIR"MyLinuxEtc"
APP_BACKUP_DIR=$GIT_SOURCEDIR"JustForBackUp"

echo "Pull Server Info ..."

cd ~

arg_init="init"
if [ $arg_init = "$1" ]; then
	echo "This is the frist time to backup."
	echo "The apps directory will be backuped."
	tar -cvf $APP_BACKUP_DIR/apps.tar.gz apps
fi

cp -rf ~/Mac_notes $BACKUP_DIR
cp ~/backup.sh $BACKUP_DIR/backup_mac.sh
cp -rf ~/.ssh $BACKUP_DIR/ssh/mac_ssh
cp -rf ~/.vimrc $BACKUP_DIR/vimrc

#==========================================

for ff in $(ls $GIT_SOURCEDIR); do
	cd $GIT_SOURCEDIR"$ff"
	echo "***************************"
	echo "Change Directory: `pwd`"

	if [ "$ff" = "bgdaemon" ]; then
		git checkout develop
		git add -A
		git commit -m"just for backup"
		git push origin develop
	else
		echo "Pull source from GitHub..."
		git pull origin master
		echo "Push local source to GitHub..."
		git add -A
		git commit -m"just for backup"
		git push origin master
	fi
	echo "  "
done
cd ~

echo "Finish Backup ..."


