#include <iostream>

#include <cstring>

using namespace std;

union Foo {
	enum class Type {
		INT,
		FLOAT
	}type;
	struct {
		Type type;
		int i;
	}i;
	struct {
		Type type;
		float f;
	}f;
};

void serialize(Foo* f, void* buffer) {
	int len = 0;
	memcpy((void*)((int)buffer + len), &f->type, sizeof(Foo::Type));
	len += sizeof(Foo::Type);
	switch(f->type) {
		case Foo::Type::INT:
			memcpy((void*)((int)buffer + len), &f->i.i, sizeof(int));
			len += sizeof(int);
		break;
		case Foo::Type::FLOAT:
			memcpy((void*)((int)buffer + len), &f->f.f, sizeof(float));
			len += sizeof(float);
		break;
	}
}

void deserialize(Foo* f, void* buffer) {
	int len = 0;
	memcpy(&f->type, (void*)((int)buffer + len), sizeof(Foo::Type));
	len += sizeof(Foo::Type);
	switch(f->type) {
		case Foo::Type::INT:
			memcpy(&f->i.i, (void*)((int)buffer + len), sizeof(int));
			len += sizeof(int);
		break;
		case Foo::Type::FLOAT:
			memcpy(&f->f.f, (void*)((int)buffer + len), sizeof(float));
			len += sizeof(float);
		break;
	}
}

int main() {
	Foo f;
	f.type = Foo::Type::FLOAT;
	f.f.f = 3.14;
	Foo i;
	i.type = Foo::Type::INT;
	i.i.i = 42;

	char buffer[sizeof(Foo)];
	serialize(&f, buffer);
	deserialize(&i, buffer);

	switch(i.type) {
		case Foo::Type::INT:
			cout << i.i.i << endl;
		break;
		case Foo::Type::FLOAT:
			cout << i.f.f << endl;
		break;
	}

	return 0;
}