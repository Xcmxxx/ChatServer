#变量 当前目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./redis REDIS_LIST)



#执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${REDIS_LIST})

#库文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)



