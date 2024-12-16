// ***************************************************
// * CS460: Programming Assignment 6: Test Program 4 *
// ***************************************************



// *******************************************************************************************
// * The factorial procedure calculates the factorial of a given number recursively.         *
// *                                                                                         *
// * If n is 0, return 1 (base case).                                                        *
// * Otherwise, return n * factorial(n - 1).                                                *
// *******************************************************************************************
procedure factorial (int n)
{
  if (n == 0)
  {
    return 1;
  }
  else
  {
    return n * factorial(n - 1);
  }
}



// *******************************************************************************************
// * Main program to test the recursive factorial function.                                  *
// *******************************************************************************************
procedure main (void)
{
  int result;
  int number;

  // Test with a specific number
  number = 5;

  result = factorial(number);
  
  printf("Factorial of %d is %d\n", number, result);
}
