// time c functions
clock_t t0 = clock();
run stuff here
printf("time: %f\n", (double) (clock()-t0)/CLOCKS_PER_SEC);
t0 = clock();
