//
//  owen.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/18/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_OWEN_HPP
#define NP_ENGINE_OWEN_HPP

#include "Primitive/Primitive.hpp"

#include "math_functions.hpp"

namespace np
{
    namespace math
    {
        namespace _hidden //TODO: refactor all hidden namespaces to the same... should we use "_hidden" or "hidden"?
        {
            /*
             John Burkardt owen T
             
             Implementation of Owen's T function follows Burkardt's implementation
             found here: <https://people.sc.fsu.edu/~jburkardt/cpp_src/owen/owen.html>
             
             Changes include formatting and refactoring.
             */
            
            
            /**
             ZNORM1 evaluates the normal CDF from 0 to Z.
             
             Input, dbl Z, the upper limit.
             
             Output, dbl ZNORM1, the probability that a standard normal variable will lie between 0 and Z.
             */
            static inline dbl znorm1(dbl z)
            {
                return 0.5 * erf(z / sqrt(2.0)); //TODO: we need to use our erf, not c++'s
            }
            
            /**
             ZNORM2 evaluates the normal CDF from Z to +oo.
             
             Input, dbl Z, the lower limit.
             
             Output, dbl ZNORM2, the probability that a standard normal variable will lie between Z and +oo.
             */
            static inline dbl znorm2(dbl z)
            {
                return 0.5 * erfc(z / sqrt(2.0));
            }
            
            /**
             TFUN computes Owen's T function for a restricted range of parameters.
             
             This routine, originally named "TF", was renamed "TFUN" to avoid
             a conflict with a built in MATLAB function.
             
             Thanks to Marko Javenpaa for identifying an incorrect modification of
             the code that resulting in halving the output of ZNORM1 for algorithms
             T2 and T3.
             
             Input, dbl H, the H argument of the function.
             0 <= H.
             
             Input, dbl A, the A argument of the function.
             0 <= A <= 1.
             
             Input, dbl AH, the value of A*H.
             
             Output, dbl TF, the value of Owen's T function.
             */
            static inline dbl tfun(dbl h, dbl a, dbl ah)
            {
                dbl ai;
                dbl aj;
                dbl arange[7] =
                {
                    0.025, 0.09, 0.15, 0.36, 0.5, 0.9, 0.99999
                    
                };
                dbl as;
                dbl c2[21] =
                {
                    0.99999999999999987510,
                    -0.99999999999988796462,      0.99999999998290743652,
                    -0.99999999896282500134,      0.99999996660459362918,
                    -0.99999933986272476760,      0.99999125611136965852,
                    -0.99991777624463387686,      0.99942835555870132569,
                    -0.99697311720723000295,      0.98751448037275303682,
                    -0.95915857980572882813,      0.89246305511006708555,
                    -0.76893425990463999675,      0.58893528468484693250,
                    -0.38380345160440256652,      0.20317601701045299653,
                    -0.82813631607004984866E-01,  0.24167984735759576523E-01,
                    -0.44676566663971825242E-02,  0.39141169402373836468E-03
                };
                dbl dhs;
                dbl dj;
                dbl gj;
                dbl hrange[14] =
                {
                    0.02, 0.06, 0.09, 0.125, 0.26,
                    0.4,  0.6,  1.6,  1.7,   2.33,
                    2.4,  3.36, 3.4,  4.8
                };
                dbl hs;
                i32 i;
                i32 iaint;
                i32 icode;
                i32 ihint;
                i32 ii;
                i32 j;
                i32 jj;
                i32 m;
                i32 maxii;
                i32 meth[18] =
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 4, 4, 4, 4, 5, 6
                };
                dbl normh;
                i32 ord[18] =
                {
                    2, 3, 4, 5, 7,10,12,18,10,20,30,20, 4, 7, 8,20,13, 0
                };
                dbl pts[13] =
                {
                    0.35082039676451715489E-02,
                    0.31279042338030753740E-01,  0.85266826283219451090E-01,
                    0.16245071730812277011,      0.25851196049125434828,
                    0.36807553840697533536,      0.48501092905604697475,
                    0.60277514152618576821,      0.71477884217753226516,
                    0.81475510988760098605,      0.89711029755948965867,
                    0.95723808085944261843,      0.99178832974629703586
                };
                dbl r;
                dbl rrtpi = 0.39894228040143267794;
                dbl rtwopi = 0.15915494309189533577;
                i32 select[15*8] =
                {
                    1, 1, 2,13,13,13,13,13,13,13,13,16,16,16, 9,
                    1, 2, 2, 3, 3, 5, 5,14,14,15,15,16,16,16, 9,
                    2, 2, 3, 3, 3, 5, 5,15,15,15,15,16,16,16,10,
                    2, 2, 3, 5, 5, 5, 5, 7, 7,16,16,16,16,16,10,
                    2, 3, 3, 5, 5, 6, 6, 8, 8,17,17,17,12,12,11,
                    2, 3, 5, 5, 5, 6, 6, 8, 8,17,17,17,12,12,12,
                    2, 3, 4, 4, 6, 6, 8, 8,17,17,17,17,17,12,12,
                    2, 3, 4, 4, 6, 6,18,18,18,18,17,17,17,12,12
                };
                dbl value;
                dbl vi;
                dbl wts[13] =
                {
                    0.18831438115323502887E-01,
                    0.18567086243977649478E-01,  0.18042093461223385584E-01,
                    0.17263829606398753364E-01,  0.16243219975989856730E-01,
                    0.14994592034116704829E-01,  0.13535474469662088392E-01,
                    0.11886351605820165233E-01,  0.10070377242777431897E-01,
                    0.81130545742299586629E-02,  0.60419009528470238773E-02,
                    0.38862217010742057883E-02,  0.16793031084546090448E-02
                };
                dbl y;
                dbl yi;
                dbl z;
                dbl zi;
                //
                //  Determine appropriate method from t1...t6
                //
                ihint = 15;
                
                for (i = 1; i <= 14; i++)
                {
                    if (h <= hrange[i-1])
                    {
                        ihint = i;
                        break;
                    }
                }
                
                iaint = 8;
                
                for (i = 1; i <= 7; i++)
                {
                    if (a <= arange[i-1])
                    {
                        iaint = i;
                        break;
                    }
                }
                
                icode = select[ihint-1+(iaint-1)*15];
                m = ord[icode-1];
                //
                //  t1(h, a, m) ; m = 2, 3, 4, 5, 7, 10, 12 or 18
                //  jj = 2j - 1 ; gj = exp(-h*h/2) * (-h*h/2)**j / j//
                //  aj = a**(2j-1) / (2*pi)
                //
                if (meth[icode-1] == 1)
                {
                    hs = - 0.5 * h * h;
                    dhs = exp(hs);
                    as = a * a;
                    j = 1;
                    jj = 1;
                    aj = rtwopi * a;
                    value = rtwopi * atan(a);
                    dj = dhs - 1.0;
                    gj = hs * dhs;
                    
                    for (;;)
                    {
                        value = value + dj * aj / (dbl)(jj);
                        
                        if (m <= j)
                        {
                            return value;
                        }
                        j = j + 1;
                        jj = jj + 2;
                        aj = aj * as;
                        dj = gj - dj;
                        gj = gj * hs / (dbl)(j);
                    }
                }
                //
                //  t2(h, a, m) ; m = 10, 20 or 30
                //  z = (-1)**(i-1) * zi ; ii = 2i - 1
                //  vi = (-1)**(i-1) * a**(2i-1) * exp[-(a*h)**2/2] / sqrt(2*pi)
                //
                else if (meth[icode-1] == 2)
                {
                    maxii = m + m + 1;
                    ii = 1;
                    value = 0.0;
                    hs = h * h;
                    as = - a * a;
                    vi = rrtpi * a * exp(-0.5 * ah * ah);
                    z = znorm1(ah) / h;
                    y = 1.0 / hs;
                    
                    for (;;)
                    {
                        value = value + z;
                        
                        if (maxii <= ii)
                        {
                            value = value * rrtpi * exp(-0.5 * hs);
                            return value;
                        }
                        z = y * (vi - (dbl)(ii) * z);
                        vi = as * vi;
                        ii = ii + 2;
                    }
                }
                //
                //  t3(h, a, m) ; m = 20
                //  ii = 2i - 1
                //  vi = a**(2i-1) * exp[-(a*h)**2/2] / sqrt(2*pi)
                //
                else if (meth[icode-1] == 3)
                {
                    i = 1;
                    ii = 1;
                    value = 0.0;
                    hs = h * h;
                    as = a * a;
                    vi = rrtpi * a * exp(-0.5 * ah * ah);
                    zi = znorm1(ah) / h;
                    y = 1.0 / hs;
                    
                    for (;;)
                    {
                        value = value + zi * c2[i-1];
                        
                        if (m < i)
                        {
                            value = value * rrtpi * exp(-0.5 * hs);
                            return value;
                        }
                        zi = y  * ((dbl) (ii) * zi - vi);
                        vi = as * vi;
                        i = i + 1;
                        ii = ii + 2;
                    }
                }
                //
                //  t4(h, a, m) ; m = 4, 7, 8 or 20;  ii = 2i + 1
                //  ai = a * exp[-h*h*(1+a*a)/2] * (-a*a)**i / (2*pi)
                //
                else if (meth[icode-1] == 4)
                {
                    maxii = m + m + 1;
                    ii = 1;
                    hs = h * h;
                    as = - a * a;
                    value = 0.0;
                    ai = rtwopi * a * exp(-0.5 * hs * (1.0 - as));
                    yi = 1.0;
                    
                    for (;;)
                    {
                        value = value + ai * yi;
                        
                        if (maxii <= ii)
                        {
                            return value;
                        }
                        ii = ii + 2;
                        yi = (1.0 - hs * yi) / (dbl) (ii);
                        ai = ai * as;
                    }
                }
                //
                //  t5(h, a, m) ; m = 13
                //  2m - point gaussian quadrature
                //
                else if (meth[icode-1] == 5)
                {
                    value = 0.0;
                    as = a * a;
                    hs = - 0.5 * h * h;
                    for (i = 1; i <= m; i++)
                    {
                        r = 1.0 + as * pts[i-1];
                        value = value + wts[i-1] * exp(hs * r) / r;
                    }
                    value = a * value;
                }
                //
                //  t6(h, a);  approximation for a near 1, (a<=1)
                //
                else if (meth[icode-1] == 6)
                {
                    normh = znorm2(h);
                    value = 0.5 * normh * (1.0 - normh);
                    y = 1.0 - a;
                    r = atan(y / (1.0 + a));
                    
                    if (r != 0.0)
                    {
                        value = value - rtwopi * r * exp(-0.5 * y * h * h / r);
                    }
                }
                return value;
            }
            
            
        }
        
        /**
         T computes Owen's T function for arbitrary H and A.
         
         Input, dbl H, A, the arguments.
         
         Output, dbl T, the value of Owen's T function.
         */
        static inline dbl owen_t(dbl h, dbl a)
        {
            dbl absa;
            dbl absh;
            dbl ah;
            dbl cut = 0.67;
            dbl normah;
            dbl normh;
            dbl value;
            
            absh = fabs (h);
            absa = fabs (a);
            ah = absa * absh;
            
            if (absa <= 1.0)
            {
                value = _hidden::tfun(absh, absa, ah);
            }
            else if (absh <= cut)
            {
                value = 0.25 - _hidden::znorm1(absh) * _hidden::znorm1(ah)
                -_hidden::tfun(ah, 1.0 / absa, absh);
            }
            else
            {
                normh = _hidden::znorm2(absh);
                normah = _hidden::znorm2(ah);
                value = 0.5 * (normh + normah) - normh * normah
                -_hidden::tfun(ah, 1.0 / absa, absh);
            }
            
            if (a < 0.0)
            {
                value = - value;
            }
            
            return value;
        }
    }
}

#endif /* NP_ENGINE_OWEN_HPP */
