#!/bin/sh
usage(){
	echo "***************************************************************************"
	echo "Usage: $0 db_path   (db_path should be absolutely path)"
	echo "Example: $0 /data/bussale/test.db"
	echo ""
	echo "If you want use this script from other application,"
	echo "you must call 'cd' command to change directory into the script directory"
	echo "then call this script."
	echo "ATTENTION: sql script must with lower case suffix:.sql"
	echo "***************************************************************************"
}

##############################################
# get the database path from paramater
db_path="/data/bussale/busdb.sqlite"
if [[ $# -lt 1 ]]; then
	echo "must give the database path"
	usage
	exit 1
fi
db_path="$1"
echo "get db_path:$db_path"

MAX_TIMES=100
##############################################
# define the function

# execute MAX_TIMES under the "tmp" directory
handle_tmp_directory(){
	cd tmp
	while [[ true ]]; do
		i=0
		file_num=0
		for sqlf in $(ls .); do
			file_num=$(( file_num+1 ))
			try_times=$(( ${sqlf%%.*} ))
			echo "$sqlf has try $try_times times"
			if [[ $try_times -lt $MAX_TIMES ]]; then
				i=$(( i+1 ))
				sqlite3 $db_path < $sqlf
				if [[ $? -ne 0 ]]; then
					echo "exec $sqlf failed"
					sql_name=${sqlf#*.}
					try_times=$(( try_times+1 ))
					ln -s ../$sql_name $try_times.$sql_name
				fi
				rm -f $sqlf
			fi
		done
		if [[ $i -eq 0 ]]; then
			cd ..
			if [[ $file_num -eq 0 ]]; then
				return 0
			fi
			return 1
		fi
	done
} 

# execute erevry sql script under special directory
exec_sql_func(){
	cd $1
	mkdir tmp > /dev/null 2>&1
	for sql_src in $(ls .); do
		if [ "${sql_src#*.}" = "sql" ] ; then
			echo "executing $sql_src"
			sqlite3 $db_path < $sql_src
			if [[ $? -ne 0 ]]; then
				cd tmp
				ln -s ../$sql_src 1.$sql_src
				cd ..
			fi
		fi
	done
	handle_tmp_directory
	if [[ $? -ne 0 ]]; then
		rm -rf tmp
		cd ..
		return 1
	fi
	rm -rf tmp
	cd ..
	return 0
}

##############################################
# start the main rotate

echo "enter table directory..."
exec_sql_func "table"
if [[ $? -ne 0 ]]; then
	return 1
fi

echo "enter view directory..."
exec_sql_func "view"
if [[ $? -ne 0 ]]; then
	return 1
fi

echo "enter trigger directory..."
exec_sql_func "trigger"
if [[ $? -ne 0 ]]; then
	return 1
fi

echo "enter index directory..."
exec_sql_func "index"
if [[ $? -ne 0 ]]; then
	return 1
fi

echo "enter insert directory..."
exec_sql_func "insert"
if [[ $? -ne 0 ]]; then
	return 1
fi

return 0

echo "finish the sql script"

