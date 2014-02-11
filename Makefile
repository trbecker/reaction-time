reaction-time: src/sqlite3.o src/reaction_time.o src/sqlite_helper.o
	gcc -o reaction-time src/sqlite3.o src/reaction_time.o src/sqlite_helper.o -lpthread -ldl -lcurses


