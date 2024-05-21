template<typename T>
class threadpool
{
public:     
            
private:
    static void* worker(void *args);
    int a;
    int b;
};

template<typename T>
void* threadpool<T>::worker(void *args)
{
a=2;
b=3;
}




int main()
{
    int *a=new int(5);
threadpool<int>::worker(a);



}