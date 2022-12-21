//-1 for [Divide integers as strings](https://www.codewars.com/kata/58dea43ff98a7e2124000169)
//0. from https://gist.github.com/shwangdev/1330739
//1.delete throw() for c++1z and later 
//2.use auto whenever possible

#include <deque>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;
class BigInteger
{
private:
  // char是为了省内存,用string存ctor(str)效果相同且debug时候依旧不可见,根本原因是计算时候需要整数而不是string里的char所存的ASCII值.
  vector<char> digits;  //in reverse order.
  bool sign;          //  true for positive, false for negitive
  void trim();        //  remove zeros in tail, but if the value is 0, keep only one.
public:
  BigInteger(int);    // construct with a int integer
  BigInteger(string&);  //~ with a string
  BigInteger();
  BigInteger(const BigInteger&);
  BigInteger operator=(const BigInteger& op2);
  BigInteger      abs() const;
  BigInteger      pow(int a);
  string          to_string();

  //binary operators
  friend BigInteger operator+=(BigInteger&, const BigInteger&);
  friend BigInteger operator-=(BigInteger&, const BigInteger&);
  friend BigInteger operator*=(BigInteger&, const BigInteger&);
  friend BigInteger operator/=(BigInteger&, const BigInteger&);
  friend BigInteger operator%=(BigInteger&, const BigInteger&);

  friend BigInteger operator+(const BigInteger&, const BigInteger&);
  friend BigInteger operator-(const BigInteger&, const BigInteger&);
  friend BigInteger operator*(const BigInteger&, const BigInteger&);
  friend BigInteger operator/(const BigInteger&, const BigInteger&);
  friend BigInteger operator%(const BigInteger&, const BigInteger&);

  //uniary operators
  friend BigInteger operator-(const BigInteger&);   //negative
  friend BigInteger operator++(BigInteger&);        //++v
  friend BigInteger operator++(BigInteger&, int);    //v++
  friend BigInteger operator--(BigInteger&);        //--v
  friend BigInteger operator--(BigInteger&, int);    //v--

  friend bool operator>(const BigInteger&, const BigInteger&);
  friend bool operator<(const BigInteger&, const BigInteger&);
  friend bool operator==(const BigInteger&, const BigInteger&);
  friend bool operator!=(const BigInteger&, const BigInteger&);
  friend bool operator>=(const BigInteger&, const BigInteger&);
  friend bool operator<=(const BigInteger&, const BigInteger&);

  friend ostream& operator<<(ostream&, const BigInteger&);    //print the BigInteger
  friend istream& operator>>(istream&, BigInteger&);         // input the BigInteger

public:
  static const BigInteger ZERO;
  static const BigInteger ONE;
  static const BigInteger TEN;
};

const BigInteger BigInteger::ZERO = BigInteger(0);
const BigInteger BigInteger::ONE = BigInteger(1);
const BigInteger BigInteger::TEN = BigInteger(10);

BigInteger::BigInteger()
{
  sign = true;
}

BigInteger::BigInteger(int val) {// construct with a int integer
  if (val >= 0)
    sign = true;
  else {
    sign = false;
    val *= (-1);
  }
  do {
    digits.push_back((char)(val % 10));
    val /= 10;
  } while (val != 0);
}

BigInteger::BigInteger(string& def) {
  sign = true;
  for (auto iter = def.rbegin(); iter < def.rend(); iter++) {
    char ch = (*iter);
    if (iter == def.rend() - 1) {
      if (ch == '+')
        break;
      if (ch == '-') {
        sign = false;
        break;
      }
    }
    digits.push_back((char)((*iter) - '0'));
  }
  trim();
}

void BigInteger::trim() {
  auto iter = digits.rbegin();
  while (!digits.empty() && (*iter) == 0) {
    digits.pop_back();
    iter = digits.rbegin();
  }
  if (digits.size() == 0) {
    sign = true;
    digits.push_back(0);
  }
}
BigInteger::BigInteger(const BigInteger& op2) {
  sign = op2.sign;
  digits = op2.digits;
}
BigInteger BigInteger::operator=(const BigInteger& op2) {
  digits = op2.digits;
  sign = op2.sign;
  return (*this);
}
BigInteger BigInteger::abs() const {
  if (sign)  return *this;
  else      return -(*this);
}
BigInteger BigInteger::pow(int a)
{
  BigInteger res(1);
  for (int i = 0; i < a; i++)
    res *= (*this);
  return res;
}
string BigInteger::to_string()
{
  string res;
  for (auto i : digits)
    res += char(i + '0');
  reverse(begin(res), end(res));
  return res;
}

//binary operators
BigInteger operator+=(BigInteger& op1, const BigInteger& op2) {
  if (op1.sign == op2.sign) {     //只处理相同的符号的情况，异号的情况给-处理
    auto iter1 = op1.digits.begin();
    auto iter2 = op2.digits.cbegin();
    char to_add = 0;        //进位
    while (iter1 != op1.digits.end() && iter2 != op2.digits.end()) {
      (*iter1) = (*iter1) + (*iter2) + to_add;
      to_add = ((*iter1) > 9);    // 大于9进一位
      (*iter1) = (*iter1) % 10;
      iter1++; iter2++;
    }
    while (iter1 != op1.digits.end()) {
      (*iter1) = (*iter1) + to_add;
      to_add = ((*iter1) > 9);
      (*iter1) %= 10;
      iter1++;
    }
    while (iter2 != op2.digits.end()) {
      char val = (*iter2) + to_add;
      to_add = (val > 9);
      val %= 10;
      op1.digits.push_back(val);
      iter2++;
    }
    if (to_add != 0)
      op1.digits.push_back(to_add);
    return op1;
  }
  else {
    if (op1.sign)
      return op1 -= (-op2);
    else
      return op1 = op2 - (-op1);
  }
}
BigInteger operator-=(BigInteger& op1, const BigInteger& op2) {
  if (op1.sign == op2.sign) {
    if (op1.sign) {
      if (op1 < op2)
        return  op1 = -(op2 - op1);
    }
    else {
      if (-op1 > -op2)
        return op1 = -((-op1) - (-op2));
      else
        return op1 = (-op2) - (-op1);
    }
    auto iter1 = op1.digits.begin();
    auto iter2 = op2.digits.cbegin();

    char to_substract = 0;

    while (iter1 != op1.digits.end() && iter2 != op2.digits.end()) {
      (*iter1) = (*iter1) - (*iter2) - to_substract;
      to_substract = 0;
      if ((*iter1) < 0) {
        to_substract = 1;
        (*iter1) += 10;
      }
      iter1++;
      iter2++;
    }
    while (iter1 != op1.digits.end()) {
      (*iter1) = (*iter1) - to_substract;
      to_substract = 0;
      if ((*iter1) < 0) {
        to_substract = 1;
        (*iter1) += 10;
      }
      else break;
      iter1++;
    }
    op1.trim();
    return op1;
  }
  else {
    if (op1 > BigInteger::ZERO)
      return op1 += (-op2);
    else
      return op1 = -(op2 + (-op1));
  }
}
BigInteger operator*=(BigInteger& op1, const BigInteger& op2) {
  BigInteger result(0);
  if (op1 == BigInteger::ZERO || op2 == BigInteger::ZERO)
    result = BigInteger::ZERO;
  else {
    auto iter2 = op2.digits.cbegin();
    while (iter2 != op2.digits.end()) {
      if (*iter2 != 0) {
        deque<char> temp(op1.digits.begin(), op1.digits.end());
        char to_add = 0;
        deque<char>::iterator iter1 = temp.begin();
        while (iter1 != temp.end()) {
          (*iter1) *= (*iter2);
          (*iter1) += to_add;
          to_add = (*iter1) / 10;
          (*iter1) %= 10;
          iter1++;
        }
        if (to_add != 0)
          temp.push_back(to_add);
        int num_of_zeros = iter2 - op2.digits.begin();
        while (num_of_zeros--)
          temp.push_front(0);
        BigInteger temp2;
        temp2.digits.insert(temp2.digits.end(), temp.begin(), temp.end());
        temp2.trim();
        result = result + temp2;
      }
      iter2++;
    }
    result.sign = ((op1.sign && op2.sign) || (!op1.sign && !op2.sign));
  }
  op1 = result;
  return op1;
}
BigInteger operator/=(BigInteger& op1, const BigInteger& op2) {
  if (op2 == BigInteger::ZERO)
  {
    cout << "error:devided by zero!" << endl;
    return BigInteger::ZERO;
  }

  BigInteger t1 = op1.abs(), t2 = op2.abs();
  if (t1 < t2) {
    op1 = BigInteger::ZERO;
    return op1;
  }
  deque<char> temp;
  auto iter = t1.digits.rbegin();
  BigInteger temp2(0);
  while (iter != t1.digits.rend()) {
    temp2 = temp2 * BigInteger::TEN + BigInteger((int)(*iter));
    char s = 0;
    while (temp2 >= t2) {
      temp2 = temp2 - t2;
      s = s + 1;
    }
    temp.push_front(s);
    iter++;
  }
  op1.digits.clear();
  op1.digits.insert(op1.digits.end(), temp.begin(), temp.end());
  op1.trim();
  op1.sign = ((op1.sign && op2.sign) || (!op1.sign && !op2.sign));
  return op1;
}
BigInteger operator%=(BigInteger& op1, const BigInteger& op2) {
  return op1 -= ((op1 / op2) * op2);
}
BigInteger operator+(const BigInteger& op1, const BigInteger& op2) {
  BigInteger temp(op1);
  temp += op2;
  return temp;
}
BigInteger operator-(const BigInteger& op1, const BigInteger& op2) {
  BigInteger temp(op1);
  temp -= op2;
  return temp;
}
BigInteger operator*(const BigInteger& op1, const BigInteger& op2) {
  BigInteger temp(op1);
  temp *= op2;
  return temp;

}
BigInteger operator/(const BigInteger& op1, const BigInteger& op2) {
  BigInteger temp(op1);
  temp /= op2;
  return temp;
}
BigInteger operator%(const BigInteger& op1, const BigInteger& op2) {
  BigInteger temp(op1);
  temp %= op2;
  return temp;
}

BigInteger operator-(const BigInteger& op) {
  BigInteger temp = BigInteger(op);
  temp.sign = !temp.sign;
  return temp;
}
BigInteger operator++(BigInteger& op) {
  op += BigInteger::ONE;
  return op;
}
BigInteger operator++(BigInteger& op, int x) {
  BigInteger temp(op);
  ++op;
  return temp;
}
BigInteger operator--(BigInteger& op) {
  op -= BigInteger::ONE;
  return op;
}
BigInteger operator--(BigInteger& op, int x) {
  BigInteger temp(op);
  --op;
  return temp;
}

bool operator<(const BigInteger& op1, const BigInteger& op2) {
  if (op1.sign != op2.sign)
    return !op1.sign;
  else {
    if (op1.digits.size() != op2.digits.size())
      return (op1.sign && op1.digits.size() < op2.digits.size())
      || (!op1.sign && op1.digits.size() > op2.digits.size());
    auto iter1 = op1.digits.crbegin();
    auto iter2 = op2.digits.crbegin();
    while (iter1 != op1.digits.rend()) {
      if (op1.sign && *iter1 < *iter2) return true;
      if (op1.sign && *iter1 > *iter2) return false;
      if (!op1.sign && *iter1 > *iter2) return true;
      if (!op1.sign && *iter1 < *iter2) return false;
      iter1++;
      iter2++;
    }
    return false;
  }
}
bool operator==(const BigInteger& op1, const BigInteger& op2) {
  if (op1.sign != op2.sign || op1.digits.size() != op2.digits.size())
    return false;
  auto iter1 = op1.digits.cbegin();
  auto iter2 = op2.digits.cbegin();
  while (iter1 != op1.digits.end()) {
    if (*iter1 != *iter2)  return false;
    iter1++;
    iter2++;
  }
  return true;
}
bool operator!=(const BigInteger& op1, const BigInteger& op2) {
  return !(op1 == op2);
}
bool operator>=(const BigInteger& op1, const BigInteger& op2) {
  return (op1 > op2) || (op1 == op2);
}
bool operator<=(const BigInteger& op1, const BigInteger& op2) {
  return (op1 < op2) || (op1 == op2);
}
bool operator>(const BigInteger& op1, const BigInteger& op2) {
  return !(op1 <= op2);
}
ostream& operator<<(ostream& stream, const BigInteger& val) {    //print the BigInteger
  if (!val.sign)
    stream << "-";
  for (auto iter = val.digits.crbegin(); iter != val.digits.rend(); iter++)
    stream << (char)((*iter) + '0');
  return stream;
}
istream& operator>>(istream& stream, BigInteger& val) {    //Input the BigInteger
  string str;
  stream >> str;
  val = BigInteger(str);
  return stream;
}

BigInteger gcd(BigInteger m, BigInteger n)
{
  if (m < n)
    std::swap(m, n);
  if (n == BigInteger(0))
    return m;
  return gcd(n, m % n);
}
BigInteger lcm(BigInteger m, BigInteger n)
{
  return m * n / gcd(m, n);
}

vector<string> divide_strings(string a, string b) {
  BigInteger ba(a);
  BigInteger bb(b);
  auto quotient = ba / bb;
  auto remainder = ba - bb * quotient;
  auto s1 = remainder.to_string();
  auto s2 = quotient.to_string();
  return vector<string>{s2, s1};
}
int main()
{
  //  std::string b1, b2;
    //do
    //{
    //  std::cin >> b1 >> b2;
    //  if (b1 == b2 && b1 == "0")
    //    break;
    //  BigInteger ret = lcm(BigInteger(b1), BigInteger(b2));
    //  std::cout << ret << std::endl;
    //} while (1);
  string b1 = "600001";
  string b2 = "100";
  cout << divide_strings(b1, b2);
}
