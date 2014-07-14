/*
class e
{
};
class x: public e
{
};
struct y;
enum z;

x foox();
y fooy();
z fooz();
*/

class A {};
class B : public A {};
typedef A* C;

const bool *ab(int a, int b);

bool const *abc(int a, int b);

bool *const abcd(int a, int b);

int main();
void foo();
int sum(int a, int b);
int diff(int a, int b);
