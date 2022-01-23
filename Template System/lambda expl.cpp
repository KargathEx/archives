
/*
 * Lambda Calculus Interpreter in the C++ Template System
 *
 * Copyright 2007, K.D.P.Ross <kyle@osl.iu.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 */

// 来源: https://web.archive.org/web/20100614034135/http://www.osl.iu.edu/~kyross/academe/templ-interp/interp.cpp
#include <iostream>
#include <list>
#include <string>

using namespace std;
// {{{ Interpreter

// {{{ Program representations

/* Peano Integers */
struct Zero {};

template <typename X>
struct Succ {};

/* characters */
template <unsigned char C>
struct Char {};

/* polymorphic lists */
struct Nil {};

template <typename X, typename Xs>
struct Cons
{
    typedef X  head;
    typedef Xs tail;
};

/* abstract syntax trees */
/* T1 + T2 */
template <typename T1, typename T2>
struct Sum {};

template <typename T1, typename T2>
struct Cat {};

template <unsigned int X>
struct Var {};

/* lambda X . T */
template <unsigned int X, typename T>
struct Abst {};

/* T1 T2 */
template <typename T1, typename T2>
struct Appl {};

/* let X = T1 in T2 */
template <unsigned int X, typename T1, typename T2>
struct Let {};

/* bools */
struct True {};
struct False {};

/* X == Y */
template <typename X, typename Y>
struct Eq {};

template <typename Env, typename T>
struct Eval {};

/* if C then T else E */
template <typename C, typename T, typename E>
struct If {};

/* pred X */
template <typename X>
struct Pred {};

// }}}

// {{{ Utility / helper functions

/* type equality */
template <typename X, typename Y>
struct eq_types {};

template <typename X>
struct eq_types <X, X>
{
    typedef X res;
};

/* Peano addition */
template <typename X, typename Y>
struct PrimAdd {};

template <>
struct PrimAdd <Zero, Zero>
{
    typedef Zero res;
};

template <typename Z>
struct PrimAdd <Succ <Z>, Zero>
{
    typedef Succ <Z> res;
};

template <typename Z>
struct PrimAdd <Zero, Succ <Z> >
{
    typedef Succ <Z> res;
};

template <typename Z1, typename Z2>
struct PrimAdd <Succ <Z1>, Succ <Z2> >
{
    typedef typename PrimAdd <Succ <Succ <Z1> >, Z2> ::res res;
};

/* String concatenation */
template <typename S1, typename S2>
struct PrimCatHelp {};

template <typename S>
struct PrimCatHelp <Nil, S>
{
    typedef S res;
};

template <unsigned char C, typename S1, typename S2>
struct PrimCatHelp <Cons <Char <C>, S1>, S2>
{
    typedef typename PrimCatHelp <S1, Cons <Char <C>, S2> > ::res res;
};

template <typename S1, typename S2>
struct RevHelp {};

template <typename S>
struct RevHelp <Nil, S>
{
    typedef S res;
};

template <unsigned char C, typename S1, typename S2>
struct RevHelp <Cons <Char <C>, S1>, S2>
{
    typedef typename RevHelp <S1, Cons <Char <C>, S2> > ::res res;
};

template <typename S>
struct Rev
{
    typedef typename RevHelp <S, Nil> ::res res;
};

/* reverse the first argument, then copy it over */
template <typename S1, typename S2>
struct PrimCat
{
    typedef typename PrimCatHelp <typename Rev <S1> ::res, S2> ::res res;
};

/* user-level equality (False on failure) */
template <typename T1, typename T2>
struct PrimEq
{
    typedef False res;
};

template <typename T>
struct PrimEq <T, T>
{
    typedef True res;
};

/* Peano predecessor */
template <typename X>
struct PrimPred {};

template <typename X>
struct PrimPred <Succ <X> >
{
    typedef X res;
};

/* not sure if we want this, but it's the conventional definition */
/*
template <>
struct PrimPred <Zero>
{ typedef Zero res ;};
*/

// }}}

// {{{ Evaluator

/* Nil in Val */
template <typename Env>
struct Eval <Env, Nil>
{
    typedef Nil res;
};

/* Char[C] in Val */
template <typename Env, unsigned char C>
struct Eval <Env, Char <C> >
{
    typedef Char <C> res;
};

/* Zero in Val */
template <typename Env>
struct Eval <Env, Zero>
{
    typedef Zero res;
};

/* True in Val */
template <typename E>
struct Eval <E, True>
{
    typedef True res;
};

/* False in Val */
template <typename E>
struct Eval <E, False>
{
    typedef False res;
};

/* Env |- Z ==> V
   -----------------------------------
   Env |- Succ <Z> ==> Succ <V>
*/
template <typename Env, typename Z>
struct Eval <Env, Succ <Z> >
{
    typedef Succ <typename Eval <Env, Z> ::res> res;
};

/* Env |- T1 ==> V1  Env |- T2 ==> V2
   ----------------------------------
   Env |- T1 + T2 ==> V1 <add> V2
*/
template <typename Env, typename T1, typename T2>
struct Eval <Env, Sum <T1, T2> >
{
    typedef typename PrimAdd <typename Eval <Env, T1> ::res, typename Eval <Env, T2> ::res> ::res res;
};

/* Env |- T1 ==> V1  Env |- T2 ==> V2
   ----------------------------------
   Env |- T1 ^ T2 ==> V1 <cat> V2
*/
template <typename Env, typename T1, typename T2>
struct Eval <Env, Cat <T1, T2> >
{
    typedef typename PrimCat <typename Eval <Env, T1> ::res, typename Eval <Env, T2> ::res> ::res res;
};

template <unsigned int X>
struct Eval <Nil, Var <X> > {};

/* ----------------------
   Env, <X, T> |- X ==> T
*/
template <typename T, typename Es, unsigned int X>
struct Eval <Cons <Cons <Var <X>, T>, Es>, Var <X> >
{
    typedef T res;
};

/* Env |- X ==> T'
   -----------------------
   Env, <Y, T> |- X ==> T'
*/
template <typename E, typename Es, unsigned int X>
struct Eval <Cons <E, Es>, Var <X> >
{
    typedef typename Eval <Es, Var <X> > ::res res;
};

/* Env |- T1 ==> V1  Env |- T2 ==> V2  Env |- V1 V2 ==> V
   ------------------------------------------------------
   Env |- T1 T2 ==> V
*/
template <typename E, typename T1, typename T2>
struct Eval <E, Appl <T1, T2> >
{
    typedef typename Eval <E, Appl <Eval <E, T1>, Eval <E, T2> > > ::res res;
};

/* Env |- T2 ==> V2  Env, <X, V2> |- T1 ==> V
   ------------------------------------------
   Env |- (\ X . T1) T2 ==> V
*/
template <typename E, unsigned int X, typename T1, typename T2>
struct Eval <E, Appl <Abst <X, T1>, T2> >
{
    typedef typename Eval <Cons <Cons <Var <X>, typename Eval <E, T2> ::res>, E>, T1> ::res res;
};

/* Env |- (\ X . T2) T1 ==> V
   -----------------------------
   Env |- Let X = T1 in T2 ==> V
*/
template <typename E, unsigned int X, typename T1, typename T2>
struct Eval <E, Let <X, T1, T2> >
{
    typedef typename Eval <E, Appl <Abst <X, T2>, T1> > ::res res;
};

/* Env |- X ==> V  Env |- Xs ==> Vs
   --------------------------------
   Env |- X :: Xs ==> V :: Vs
*/
template <typename E, typename X, typename Xs>
struct Eval <E, Cons <X, Xs> >
{
    typedef Cons <typename Eval <E, X> ::res, typename Eval <E, Xs> ::res> res;
};

/* Env |- T1 ==> V1  Env |- T2 ==> V2
   ----------------------------------
   Env |- T1 == T2 ==> V1 <equals> V2
*/
template <typename E, typename T1, typename T2>
struct Eval <E, Eq <T1, T2> >
{
    typedef typename PrimEq <typename Eval <E, T1> ::res, typename Eval <E, T2> ::res> ::res res;
};

// Don't like this much -KDPR
/* Env |- T2 ==> V2
   -------------------------------------
   Env |- If True then T2 else T3 ==> V2
*/
template <typename E, typename T2, typename T3>
struct Eval <E, If <True, T2, T3> >
{
    typedef typename Eval <E, T2> ::res res;
};

/* Env |- T3 ==> V3
   -------------------------------------
   Env |- If False then T2 else T3 ==> V3
*/
template <typename E, typename T2, typename T3>
struct Eval <E, If <False, T2, T3> >
{
    typedef typename Eval <E, T3> ::res res;
};

/* Env |- T1 ==> V1  Env |- If V1 then T2 else T3 ==> V
   ----------------------------------------------------
   Env |- If T1 then T2 else T3 ==> V
*/
template <typename E, typename T1, typename T2, typename T3>
struct Eval <E, If <T1, T2, T3> >
{
    typedef typename Eval <E, If <typename Eval <E, T1> ::res, T2, T3> > ::res res;
};

/* Env |- T ==> Succ (V)
   ---------------------
   Env |- Pred (T) ==> V
*/
template <typename E, typename T>
struct Eval <E, Pred <T> >
{
    typedef typename PrimPred <typename Eval <E, T> ::res> ::res res;
};

// }}}

// }}}

// {{{ Conversions to run-time values

// {{{ Mapping between compile-time values and run-time types

/* We want to be able to take our compile-time-computed values and convert them
   to "appropriate" run-time values. To do this in a somewhat-generalisable way
   (e.g., to define how to convert a compile-time list-of-T to a run-time
   list-of-<something> in general, assuming we can convert a T). We'll define a
   traits class where we'll define the mapping between a compile-time value and
   the run-time type into which it will be projected.
*/
template <typename T>
struct type_traits {};

template <>
struct type_traits <Zero>
{
    typedef unsigned int rtt;
};

template <typename X>
struct type_traits <Succ <X> >
{
    typedef unsigned int rtt;
};

/* have to pick an arbitrary type for [] */
template <>
struct type_traits <Nil>
{
    typedef list <unsigned int> rtt;
};

/* convert list to correct run-time list */
template <typename X>
struct type_traits <Cons <X, Nil> >
{
    typedef list <typename type_traits <X> ::rtt> rtt;
};

template <typename X, typename Xs>
struct type_traits <Cons <X, Xs> >
{
    typedef list <typename eq_types <typename type_traits <X> ::rtt, typename type_traits <Xs> ::rtt::value_type> ::res> rtt;
};

/* but ... convert char lists to run-time strings */
template <unsigned char C>
struct type_traits <Cons <Char <C>, Nil> >
{
    typedef string rtt;
};

template <unsigned char C, typename Xs>
struct type_traits <Cons <Char <C>, Xs> >
{
    typedef typename eq_types <string, typename type_traits <Xs> ::rtt> ::res rtt;
};

template <unsigned char C>
struct type_traits <Char <C> >
{
    typedef char rtt;
};

template <>
struct type_traits <True>
{
    typedef bool rtt;
};

template <>
struct type_traits <False>
{
    typedef bool rtt;
};

// }}}

// {{{ Mapping between compile-time values and run-time values

/* While type_traits encodes the mapping between a(n untyped) compile-time value
   and its run-time type, reflect defines the mapping between a compile-time
   value and a run-time value. The user-level idiom to do this is:

   // runs our program
   typedef {{compile-time program}} prog ;

   // reflects the result of doing so to a run-time value of the "right" type
   type_traits <prog> :: rtt val = reflect <prog> () ;

*/

template <typename X>
typename type_traits <X> ::rtt reflect();

template <typename X>
struct reflect_help {};

/* rtt Nat = unsigned int */
template <>
struct reflect_help <Zero>
{
    static unsigned int doit()
    {
        return 0;
    }
};

template <typename X>
struct reflect_help <Succ <X> >
{
    static unsigned int doit()
    {
        return 1 + reflect <X>();
    }
};

/* rtt Char = char */
template <unsigned char C>
struct reflect_help <Char <C> >
{
    static char doit()
    {
        return C;
    }
};

/* rtt [Char] = string */
template <unsigned char C>
struct reflect_help <Cons <Char <C>, Nil> >
{
    static string doit()
    {
        string tail = " ";
        tail[0] = C;
        return tail;
    }
};

template <unsigned char C, typename Xs>
struct reflect_help <Cons <Char <C>, Xs> >
{
    static string doit()
    {
        string tail = " " + reflect <Xs>();
        tail[0] = C;
        return tail;
    }
};

/* rtt [T] = list <rtt T> ... we can, of course, use untyped lists, but we have
   no way to reflect them at run-time */
template <typename X>
struct reflect_help <Cons <X, Nil> >
{
    static list <typename type_traits <X> ::rtt> doit()
    {
        typedef typename type_traits <X> ::rtt value_type;
        const value_type head = reflect <X>();
        list <value_type> tail;
        tail.push_back(head);
        return tail;
    }
};

template <typename X, typename Xs>
struct reflect_help <Cons <X, Xs> >
{
    static typename type_traits <Cons <X, Xs> > ::rtt doit()
    {
        typedef typename type_traits <X> ::rtt value_type;
        const value_type head = reflect <X>();
        list <value_type> tail = reflect <Xs>();
        tail.push_front(head);
        return tail;
    }
};

/* rtt [\A . A] = list <unsigned int> */
template <>
struct reflect_help <Nil>
{
    static type_traits <Nil> ::rtt doit()
    {
        type_traits <Nil> ::rtt tail;
        return tail;
    }
};

/* rtt Bool = bool */
template <>
struct reflect_help <True>
{
    static type_traits <True> ::rtt doit()
    {
        return true;
    }
};

template <>
struct reflect_help <False>
{
    static type_traits <False> ::rtt doit()
    {
        return false;
    }
};

/* user can call as a function rather than dealing with typename and :: */
template <typename X>
typename type_traits <X> ::rtt reflect()
{
    return reflect_help <X> ::doit();
}

// }}}

// }}}

// {{{ Some functions for convenience

/* why aren't lists printable? */
template <typename T>
ostream& operator<< (ostream& out, list <T> xs)
{
    out << '[';
    typename list <T> ::iterator i = xs.begin();
    if (i != xs.end())
        out << *(i++);
    while (i != xs.end())
        out << ", " << *(i++);
    return out << ']';
}

/* a bit easier to call, again hides some of the implementation details */

template <typename T>
typename type_traits <typename Eval <Nil, T> ::res> ::rtt eval()
{
    return reflect <typename Eval <Nil, T> ::res>();
}

template <typename T>
struct eval_type
{
    typedef typename type_traits <typename Eval <Nil, T> ::res> ::rtt rtt;
};

 }}}

/* some little tests */
int main()
{ // 'h' : 'e' : 'l' : 'l' : 'o' : [] ==> "hello"
    cout << eval <Cons <Char <'h'>, Cons <Char <'e'>, Cons <Char <'l'>, Cons <Char <'l'>, Cons <Char <'o'>, Nil> > > > > >() << endl;
    // 2 + 3 ==> 5
    cout << eval <Sum <Succ <Succ <Zero> >, Succ <Succ <Succ <Zero> > > > >() << endl;

    // (\x . x) (1 + 1) ==> 2
    cout << eval <Appl <Abst <1u, Var <1u> >, Sum <Succ <Zero>, Succ <Zero> > > >() << endl;

    // let x = 2 + 2 in x + x ==> 8
    cout << eval <Let <1u, Sum <Succ <Succ <Zero> >, Succ <Succ <Zero> > >, Sum <Var <1u>, Var <1u> > > >() << endl;

    // ['h', 'e', 'l', 'l', 'o'] ++ ['h', 'e', 'l', 'l', 'o'] ==> "hellohello"
    cout << eval <Let <1u, Cons <Char <'h'>, Cons <Char <'e'>, Cons <Char <'l'>, Cons <Char <'l'>, Cons <Char <'o'>, Nil> > > > >, Cat <Var <1u>, Var <1u> > > >() << endl;

    // (1 + 1) : (pred (2 + 1)) : [] ==> [2, 2]
    cout << eval <Cons <Sum <Succ <Zero>, Succ <Zero> >, Cons <Pred <Sum <Succ <Succ <Zero> >, Succ <Zero> > >, Nil> > >() << endl;

    // if 0 == 1 then False else 1 + 1 ==> 2
    cout << eval <If <Eq <Zero, Succ <Zero> >, False, Sum <Succ <Zero>, Succ <Zero> > > >() << endl;

    // [] ==> []
    cout << eval <Nil>() << endl;

    return EXIT_SUCCESS;
}