# Comparison between Scalar C++ and AVX/SSE
Programming of Multimedia Extensions (SSE / AVX)
Comparison between programming on AVX intrinsics between programming on C++ scalar. The other comparison is between ICC 
and GCC compiler.

Exercise 1. Given an n*n matrix in the RGB format, transform each of its pixels to the YUV format according to the following equations on C++ & AVX:<br/>
Y = 0.299*R + 0.587*G + 0.114*B <br/>
U = -0.147*R - 0.289*G + 0.436*B = 0.492*(B-Y) <br/>
V = 0.615*R - 0.515*G - 0.100*B = 0.877*(R-Y) <br/>

Exercise 2. In the face of the following General Elections, we are charged with implementing an application 
that is capable of totalizing, as quickly as possible, the ballots sent in the hours after 
the closure of the schools by the thousands of polling stations that are scattered throughout the country.
We will simulate the introduction of the registers by means of a bidimensional array, in which the rows 
will represent the electoral tables, and the columns the votes granted to each party. The instantiation of the 
elements of this array will be done following random techniques, assigning to each party in each table a number 
of votes that is a non-negative whole value between 1 and the maximum number of votes possible in each table (100,000).

![alt text](https://i.imgur.com/CNs1Iop.png)

![alt text](https://i.imgur.com/SCHdaAE.png)
