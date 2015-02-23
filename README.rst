C++ version of this code has been taken from 
https://bitbucket.org/sreangsu/libpav/.

Idea is to write a scalable/distributed version of it using scala.

======================================================================
Comments From C++ Version:

This code provides a templated function pav() that solves for x^* where: 

 - x^* = argmin_x ||y - x||^2
 - s.t. components of x are ordered...(1)

It uses a linear time implementation of the pool adjacent vioaltors algorithm (PAV).


You can get the effect of replacing ||x - y||^2 by any other `Bregman
divergence  <http://en.wikipedia.org/wiki/Bregman_divergence>`_, by using the
transformation:  inv[\grad \phi] (x^*). The most common case is:

  - min_x 1/2 ||y - x||_W^2
  - s.t. components of x are ordered. 

If W is a diagonal matrix diag(w), the solution is obtained as x^* / sqrt(w).
Thus one need not write a special minimization code for the weighted version of
the problem. All that is needed is to transform the minimizer of (1)
appropriately. This transformation property holds with wider generality than
just for  weighted squared Euclidean distances.

Other variations that this code solves are:

lbound_margin_pav:

    - argmin_x ||x - y||^2
    - s.t. x_0           >=  m_0
    - s.t. x_i - x_{i+1} <= -m_i  \forall  i \in [1, n)


ubound_margin_pav:

    - argmin_x ||x - y||^2
    - s.t. x_{n-1}       <= -m_{n-1}
    - s.t. x_{i+1} - x_i >=  m_i  \forall  i \in [0, n-1)


lbound_maxmargin_pav:
 
    - argmin_x ||x - y||^2 - <C,m>
    - s.t. x_0           >= m_0
    - s.t. x_i - x_{i+1} <= -m_i  \forall  i \in [1, n)
    - s.t. m >= 0


ubound_maxmargin_pav:

    - argmin_x ||x - y||^2 - <C,m>
    - s.t. x_{n-1}       <= -m_{n-1}
    - s.t. x_{i+1} - x_i >=  m_i  \forall  i \in [0, n-1)
    - s.t. m >= 0

