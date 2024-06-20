EXE=fetchmail
OBJ=login.o retrieve.o parse.o mime.o list.o

$(EXE): main.c $(OBJ)
	gcc -D_GNU_SOURCE -Wall -o $(EXE) $(OBJ) $< 

%.o: %.c %.h
	gcc -c -o $@ $< -Wall

# Rust
# $(EXE): src/*.rs vendor
# 	cargo build --frozen --offline --release
# 	cp target/release/$(EXE) .

# vendor:
# 	if [ ! -d "vendor/" ]; then \
# 		cargo vendor --frozen; \
# 	fi

clean: 
	rm -f *.o $(EXE)
format:
	clang-format -style=file -i *.c *.h

