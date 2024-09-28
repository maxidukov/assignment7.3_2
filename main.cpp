#include <iostream>
#include <thread>
#include <future>
#include <vector>

template<class Iter, class Function>
void for_each_par(Iter begin, Iter end, Function func){
    int numthreads  = std::thread::hardware_concurrency();
    int block_size = (end-begin)/numthreads + 1; //  WORKS FOR RANDOM ACCESS ITERATORS, IN GENERAL std::distance(begin, end)/numthreads + 1
    std::cout << "Number of threads available is " << numthreads << ", container size is " << (end-begin) << ", block size is " << block_size << "\n";
    std::vector<std::future<void>> ftrs;
    for(int i = 0; block_size*i < end-begin; i++){
        auto lmbd = [=](){ //[&] MAKES IT WORK THE WRONG WAY: IT CAPTURES i AFTER INCREMENTING
            //std::cout << "This is happening in thread " << std::this_thread::get_id() << "\n";
            //std::cout << "This is happening in thread " << std::this_thread::get_id() << " and we start with index " << block_size*i << "\n";
            for(int j = 0; block_size*i + j < block_size*(i+1) && block_size*i + j < end-begin; ++j){
                func( *(begin + block_size*i + j) );
            }
        };
        //std::cout << "Processing elements with indices from " << block_size*i << " to " << (block_size*(i+1)-1 >= end-begin ? end-begin-1 : block_size*(i+1)-1) << "\n";

        std::future<void> ftr = std::async(lmbd); //USING ASYNC
        //std::future<void> ftr = std::async(std::launch::async, lmbd); //USING ASYNC, WITH MULTITHREAD POLICY FORCED
        //std::packaged_task<void(void)> pktsk(lmbd);                 //USING PACKAGED_TASK
        //std::future<void> ftr = pktsk.get_future();
        ftrs.push_back(std::move(ftr));
        //std::thread thrd(std::move(pktsk));
        //thrd.detach();
        //ftr.get();
    }
    for( auto& ftr : ftrs ) ftr.wait();
}


int main(){
    //std::cout << "Main thread with id " << std::this_thread::get_id() << " launched\n";
    std::vector<int> v{1,2,3,4,5,6,7,8,9};
    auto prnt = [](int num){ std::cout << num << "\n"; };
    for_each_par(v.begin(), v.end(), prnt);

    return 0;
}
