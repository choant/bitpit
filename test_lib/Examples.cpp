// ========================================================================== //
//                  - SORTING ALGORITHMS, EXAMPLES OF USAGE -                 //
//                                                                            //
// Functions for data sorting.                                                //
// ========================================================================== //
// INFO                                                                       //
// Author    : Alessandro Alaia                                               //
// Version   : v2.0                                                           //
//                                                                            //
// All rights reserved.                                                       //
// ========================================================================== //

// ========================================================================== //
// INCLUDES                                                                   //
// ========================================================================== //
# include "Examples.hpp"

// ========================================================================== //
// IMPLEMENTATIONS                                                            //
// ========================================================================== //

// -------------------------------------------------------------------------- //
void Test000(
    void
) {

// ========================================================================== //
// void Test000(                                                              //
//     void)                                                                  //
//                                                                            //
// LIFO stack demo.                                                           //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// ========================================================================== //
// VARIABLES DECLARATION                                                      //
// ========================================================================== //

// Local variables
int                            N = 10;
LIFOstack<ivector1D>           stack(5);

// Counters
int                            i;

// ========================================================================== //
// OPENING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "================== TEST 000: LIFO stack demo ==================" << endl;

}

// ========================================================================== //
// INSERT ELEMENTS INTO THE LIFO STACK                                        //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    ivector1D        dummy(2, 0);

    // PUSH elements into the LIFO stack ------------------------------------ //
    for (i = 0; i < N; i++) {
        dummy[0] = i+1;    dummy[1] = -dummy[0];
        cout << " * pushing element: " << dummy << endl;
        stack.push(dummy);
        stack.display(cout);
    } //next i

    // POP elements from the LIFO stack ------------------------------------- //
    for (i = 0; i < N; i++) {
        dummy = stack.pop();
        cout << " * popped element: " << dummy << endl;
        stack.display(cout);
    } //next i
}

// ========================================================================== //
// CLOSING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "====================== TEST 000: done!! =======================" << endl;

}

return; };

// -------------------------------------------------------------------------- //
void Test001(
    void
) {

// ========================================================================== //
// void Test001(                                                              //
//     void)                                                                  //
//                                                                            //
// FIFO stack demo.                                                           //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// ========================================================================== //
// VARIABLES DECLARATION                                                      //
// ========================================================================== //

// Local variables
int                            N = 10;
FIFOstack<int>                 stack(3);


// ========================================================================== //
// OPENING MESSAGE                                                            //
// ========================================================================== //
{

    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "================== TEST 001: LIFO stack demo ==================" << endl;

}

// ========================================================================== //
// STACK DEMO                                                                 //
// ========================================================================== //
{

    // Scope variables ------------------------------------------------------ //
    int          i;
    int          dummy;

    // Output message ------------------------------------------------------- //
    cout << " * pushing items 0, 1, 2" << endl;
    for (i = 0; i < 3; ++i) {
        stack.push(i);
    }
    stack.display(cout);

    cout << " * pushing items 3, 4, 5" << endl;
    for (i = 3; i < 6; ++i) {
        stack.push(i);
    }
    stack.display(cout);

    cout << " * popping first two items: ";
    dummy = stack.pop();
    cout << dummy << ", ";
    dummy = stack.pop();
    cout << dummy << endl;
    stack.display(cout);

    cout << " * pushing items: 6 - 17" << endl;
    for (i = 6; i < 18; ++i) {
        stack.push(i);
    } //next i
    stack.display(cout);

    cout << " * popping first 5 items: ";
    for (i = 0; i < 4; ++i) {
        dummy = stack.pop();
        cout << dummy << ", ";
    } //next i
    dummy = stack.pop();
    cout << dummy << endl;
    stack.display(cout);

    cout << " * popping remaining items: ";
    while (stack.TOPSTK > 1) {
        dummy = stack.pop();
        cout << dummy << ", ";
    } //next i
    dummy = stack.pop();
    cout << dummy << endl;
    stack.display(cout);
    
}

// ========================================================================== //
// CLOSING MESSAGE                                                            //
// ========================================================================== //
{

    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "================== TEST 001: done!! ===========================" << endl;

}


return; };

// -------------------------------------------------------------------------- //
void Test002(
    void
) {

// ========================================================================== //
// void Test002(                                                              //
//     void)                                                                  //
//                                                                            //
// Demo for minPQUEUE (min heap queue)                                        //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// ========================================================================== //
// VARIABLES DECLARATION                                                      //
// ========================================================================== //

// Local variables
ivector2D                    map(20, ivector1D(2, -1)), *map_ = &map;
minPQUEUE<double, string>    heap(2, true, map_);

// Counters
// none

// ========================================================================== //
// OPENING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "=================== TEST 002: min heap queue ==================" << endl;
}
// ========================================================================== //
// BUILD MIN-HEAP                                                             //
// ========================================================================== //
{

    // Scope variables ------------------------------------------------------ //
    int            i, N = 10;
    double         dummy_key;
    stringstream   sdummy;
    string         dummy_label;

    // Display min heap ----------------------------------------------------- //
    heap.display(cout);

    // Create min-heap ------------------------------------------------------ //
    cout << " - Creating min-heap by element insertion" << endl;
    for (i = 0; i < N; i++) {

        // Sorting key
        dummy_key = ((double) (N-i));

        // Create label
        sdummy << "i" << i;
        dummy_label = sdummy.str();
        sdummy.str("");
        map[heap.heap_size][0] = i;
        map[i][1] = i;
        heap.insert(dummy_key, dummy_label);

    } //next i
    heap.display(cout);

    // Insert new element(s) ------------------------------------------------ //
    cout << " - Inserting a new element with key: -0.1" << endl;
    dummy_key = -0.1;
    dummy_label = "i10";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);
    cout << " - Inserting a new element with key: 11.0" << endl;
    dummy_key = 11.0;
    dummy_label = "i11";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);
    cout << " - Inserting a new element with key: 9.0" << endl;
    dummy_key = 9.0;
    dummy_label = "i12";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);
    cout << " - Inserting a new element with key: 2.1" << endl;
    dummy_key = 2.1;
    dummy_label = "i13";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);

    // Modify an old element ------------------------------------------------ //
    cout << " - Modifying element with index 4" << endl;
    dummy_key = 4.1;
    dummy_label = "i8";
    heap.modify(4, dummy_key, dummy_label);
    heap.display(cout);
    cout << "mapper: " << map << endl;

    // Extract data from min-heap ------------------------------------------- //
    cout << " - Extracting data from min-heap" << endl;
    while (heap.heap_size > 0) {
        heap.extract(dummy_key, dummy_label);
        cout << "key: " << dummy_key << ", label: " << dummy_label << endl;
    } //next heap item
    heap.display(cout);
}

// ========================================================================== //
// CLOSING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "===================== TEST 002: done!! =====================" << endl;
}

return; }

// -------------------------------------------------------------------------- //
void Test003(
    void
) {

// ========================================================================== //
// void Test003(                                                              //
//     void)                                                                  //
//                                                                            //
// Demo for maxPQUEUE (max heap queue)                                        //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// ========================================================================== //
// VARIABLES DECLARATION                                                      //
// ========================================================================== //

// Local variables
ivector2D                    map(20, ivector1D(2, -1)), *map_ = &map;
maxPQUEUE<double, string>    heap(2, true, map_);

// Counters
// none

// ========================================================================== //
// OPENING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "=================== TEST 003: min heap queue ==================" << endl;
}

// ========================================================================== //
// BUILD MIN-HEAP                                                             //
// ========================================================================== //
{

    // Scope variables ------------------------------------------------------ //
    int            i, N = 10;
    double         dummy_key;
    stringstream   sdummy;
    string         dummy_label;

    // Display min heap ----------------------------------------------------- //
    heap.display(cout);

    // Create min-heap ------------------------------------------------------ //
    cout << " - Creating min-heap by element insertion" << endl;
    for (i = 0; i < N; i++) {

        // Sorting key
        dummy_key = ((double) i);

        // Create label
        sdummy << "i" << i;
        dummy_label = sdummy.str();
        sdummy.str("");
        map[heap.heap_size][0] = i;
        map[i][1] = i;
        heap.insert(dummy_key, dummy_label);

    } //next i
    heap.display(cout);

    // Insert new element(s) ------------------------------------------------ //
    cout << " - Inserting a new element with key: -0.1" << endl;
    dummy_key = -0.1;
    dummy_label = "i10";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);
    cout << " - Inserting a new element with key: 11.0" << endl;
    dummy_key = 11.0;
    dummy_label = "i11";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);
    cout << " - Inserting a new element with key: 9.0" << endl;
    dummy_key = 9.0;
    dummy_label = "i12";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);
    cout << " - Inserting a new element with key: 2.1" << endl;
    dummy_key = 2.1;
    dummy_label = "i13";
    map[heap.heap_size][0] = heap.heap_size;
    map[heap.heap_size][1] = heap.heap_size;
    heap.insert(dummy_key, dummy_label);
    heap.display(cout);

    // Modify an old element ------------------------------------------------ //
    cout << " - Modifying element with index 4" << endl;
    dummy_key = 4.1;
    dummy_label = "i8";
    heap.modify(4, dummy_key, dummy_label);
    heap.display(cout);
    cout << "mapper: " << map << endl;

    // Extract data from min-heap ------------------------------------------- //
    cout << " - Extracting data from min-heap" << endl;
    while (heap.heap_size > 0) {
        heap.extract(dummy_key, dummy_label);
        cout << "key: " << dummy_key << ", label: " << dummy_label << endl;
    } //next heap item
    heap.display(cout);
}

// ========================================================================== //
// CLOSING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "===================== TEST 003: done!! =====================" << endl;
}

return; }

// -------------------------------------------------------------------------- //
void Test004(
    void
) {

// ========================================================================== //
// void Test004(                                                              //
//     void)                                                                  //
//                                                                            //
// kd-tree demo.                                                              //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// ========================================================================== //
// VARIABLES DECLARATION                                                      //
// ========================================================================== //

// Local variables
int                         N = 5120000;
kdtree<2, double>           KD(N);
dvector2D                   X(N, dvector1D(2, 0.0));
dvector1D                   x(2, -0.1);

// Counters
// none

// ========================================================================== //
// OPENING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "=================== TEST 004: kd-tree =========================" << endl;
}


// ========================================================================== //
// BUILD KD-TREE                                                              //
// ========================================================================== //
{

    // Scope variables ------------------------------------------------------ //
    chrono::time_point<chrono::system_clock> start, end;
    int                                      i, j;
    int                                      elapsed_seconds;


    // Output message ------------------------------------------------------- //
    cout << " - Generating k-d tree by element insertion" << endl;

    // Create vertex list --------------------------------------------------- //
    srand(time(NULL));
    for (i = 0; i < N; i++) {
        X[i][0] = ((double) rand())/((double) RAND_MAX);
        X[i][1] = ((double) rand())/((double) RAND_MAX);
    } //next i

    // Build kd tree -------------------------------------------------------- //
    start = std::chrono::system_clock::now();
    for (i = 0; i < N; i++) {
        KD.insert(&X[i]);
    } //next i
    end = chrono::system_clock::now();
    elapsed_seconds = chrono::duration_cast<chrono::milliseconds>(end-start).count();
    cout << "elapsed time: " << elapsed_seconds << " ms" << endl;

    // output kd-tree ------------------------------------------------------- //
//     for (i = 0; i < KD.n_nodes; i++) {
//         cout << "j: " << i << endl;
//         cout << "target: " << *(KD.nodes[i].object_) << endl;
//         cout << "children: (L) " << KD.nodes[i].lchild_ << " (R) " << KD.nodes[i].rchild_ << endl;
//     } //next i
}

// ========================================================================== //
// INSERT ELEMENT                                                             //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    int            i, j, N = 10;

    // Output message ------------------------------------------------------- //
    cout << " - Inserting a new element" << endl;

    // Inserting a new element ---------------------------------------------- //
    KD.insert(&x);
    cout << "# of nodes: " << KD.n_nodes << endl;

    // output kd-tree ------------------------------------------------------- //
//     for (i = 0; i < KD.n_nodes; i++) {
//         cout << "j: " << i << endl;
//         cout << "target: " << *(KD.nodes[i].object_) << endl;
//         cout << "children: (L) " << KD.nodes[i].lchild_ << " (R) " << KD.nodes[i].rchild_ << endl;
//     } //next i

}

// ========================================================================== //
// CHECK FOR EXISTING ELEMENTS                                                //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    int            i, j;

    // Output message ------------------------------------------------------- //
    cout << " - Checking existing elements" << endl;

    // Create vertex list --------------------------------------------------- //

    // Build kd tree -------------------------------------------------------- //
//     for (i = N-1; i >= 0; i--) {
//         cout << "elem found: " << KD.exist(&X[i]) << endl;
//     } //next i

}

// ========================================================================== //
// CLOSING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "===================== TEST 004: done!! =====================" << endl;
}

return; }


// -------------------------------------------------------------------------- //
void Test005(
    void
) {

// ========================================================================== //
// void Test005(                                                              //
//     void)                                                                  //
//                                                                            //
// Sorting routines demo.                                                     //
// ========================================================================== //
// INPUT                                                                      //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //
// OUTPUT                                                                     //
// ========================================================================== //
// - none                                                                     //
// ========================================================================== //

// ========================================================================== //
// VARIABLES DECLARATION                                                      //
// ========================================================================== //

// Local variables
int                selection;

// Counters
// none

// ========================================================================== //
// OPENING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "=================== TEST 004: kd-tree =========================" << endl;
    cout << "Select test case:" << endl;
    cout << "'0' random extraction without replacement" << endl;
    cin  >> selection;
}

// ========================================================================== //
// EXTRACTION WITHOUT REPLACEMENT                                             //
// ========================================================================== //
if (selection == 0) {

    // Scope variables ------------------------------------------------------ //
    int               n = 10, N = 9, out;
    ivector1D         list;

    // Initialize variables ------------------------------------------------- //
    srand(time(NULL));

    // Output message ------------------------------------------------------- //
    cout << endl;
    cout << "Test: random extraction wihtout replacement" << endl;
    cout << "Subtest: random permutation of integers in [0, " << N << "]" << endl;

    // Extract n integers in [0, N] ----------------------------------------- //
    Extract_wo_Repl(n, N, list);
    cout << "Extracted: " << list << endl;
    cout << endl;

    // Output message ------------------------------------------------------- //
    cout << "Test: random extraction wihtout replacement" << endl;
    cout << "Subtest: expected value of 1 extraction in [0, " << N << "]" << endl;

    // Extract n integers in [0, N] ----------------------------------------- //
    out = 0;
    for (int I = 0; I < 100000; I++) {
        Extract_wo_Repl(1, N, list);
        out += list[0];
    } //next I
    cout << "Expectation: " << ((double) out)/(100000.0) << endl;
    cout << endl;
}

// ========================================================================== //
// CLOSING MESSAGE                                                            //
// ========================================================================== //
{
    // Scope variables ------------------------------------------------------ //
    // none

    // Output message ------------------------------------------------------- //
    cout << "===================== TEST 005: done!! =====================" << endl;
}

return; }