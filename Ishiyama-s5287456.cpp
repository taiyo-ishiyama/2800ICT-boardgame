// command line arguments must be <number of dimension><min number><max number><numbers of hint><seed>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <sstream>
#include <stack>
#include <tuple>
#include <cmath>
#include <unordered_set>
#include <bitset>
#include <chrono>

using namespace std;

class Expression
{
private:
  const string operators = "+-*/%&|^";

public:
  tuple<vector<string>, vector<string>> pick_operators(int num, int seed);
};
class Solution
{
private:
  vector<vector<string>> horizontal_ope; // store operators for rows
  vector<vector<string>> vertical_ope;   // store operators for collumns
  vector<vector<int>> results;           // store results of generated board
  vector<vector<vector<int>>> solutions; // store all possible solutions
  vector<int> vrows_count;               // store the number of all possible solutions

public:
  Solution(){};
  vector<vector<int>> initial_nums;  // the board of numbers when initiating
  int check_ope(string ope);     // check the precedence of operators
  int calc(int num1, int num2, string ope);   // claculate with 2 values with 1 operator
  int row_calc(vector<int> nums, vector<string> ope);  // calculate a result of the row
  tuple<vector<vector<string>>, vector<vector<string>>, vector<vector<int>>> get_results(Expression &exp, int num, int seed, int min, int max); // get each result of expressions for the board
  int row_solutions(int num, int min, int max, int col, int result, vector<string> row_ope);  // get all possible solutions in a row
  tuple<int, vector<vector<int>>> check_num_solutions(int num_col, int row, vector<vector<string>> board, vector<vector<bool>> valid_cell);   // check all possible solutions according to the user input
  int col_calc(int num, vector<int> count, vector<vector<vector<int>>> v_solutions);  // check all possible solutions combining with all rows
};
class Board
{
private:
  int num_col, min, max, seed_num;
  int data_type;                   // datatype of display
  bool status;                     // check whether the game is finished
  vector<vector<string>> board;    // store the current board
  vector<vector<bool>> valid_cell; // indicate whether each cell is filled
  Expression expression;
  Solution solution;

public:
  Board(int num, int min_range, int max_range, int seed);
  bool update(int row, int collumn, int value);      // update the board
  void change_datatype(int val) { data_type = val; } // change datatype of display
  void final_score(int num, int help, int time);     // generate final score
  void initial_hint(int num_hint);                   // insert hints at the beginning
  void help_next();                                  // find and insert the next valid number
  friend ostream &operator<<(ostream &os, const Board &b);
};

int main(int argc, char **argv)
{
  if (argc != 6)
  {
    cout << "Invalid arguments: <number of dimension> <min number> <max number> <numbers of hint> <seed>" << endl;
    exit(1);
  }
  const int num_dimension = stoi(argv[1]);
  const int min_range = stoi(argv[2]);
  const int max_range = stoi(argv[3]);
  const int hint = stoi(argv[4]);
  const int seed = stoi(argv[5]);
  Board game(num_dimension, min_range, max_range, seed);
  cout << game << endl;
  if (hint > 0)
  {
    game.initial_hint(hint);
    cout << game << endl;
  }
  int help_count = 0;
  bool status = false;
  auto start = chrono::high_resolution_clock::now();
  while (!status)
  {
    string row, collumn, value;
    cout << "Enter the change: ";
    cin >> row;
    if (isdigit(row[0]))
    {
      cin >> collumn >> value;
      // input validation
      if (stoi(row) > num_dimension || stoi(collumn) > num_dimension || stoi(value) > max_range || (stoi(value) < min_range && stoi(value) != 0))
      {
        cout << "invalid input" << endl;
        continue;
      }
      status = game.update(stoi(row), stoi(collumn), stoi(value));
      cout << game << endl;
    }
    else if (row == "decimal")
    {
      game.change_datatype(0);
      cout << game << endl;
    }
    else if (row == "binary")
    {
      game.change_datatype(1);
      cout << game << endl;
    }
    else if (row == "octal")
    {
      game.change_datatype(2);
      cout << game << endl;
    }
    else if (row == "hex")
    {
      game.change_datatype(3);
      cout << game << endl;
    }
    else if (row == "help")
    {
      game.help_next();
      help_count++;
      cout << game << endl;
    }
    else if (row == "quit")
    {
      break;
    }
    else
    {
      cout << "Invalid input" << endl;
    }
  }
  auto end = chrono::high_resolution_clock::now();
  int time = chrono::duration_cast<chrono::seconds>(end - start).count();
  if (status) // if game ends
    game.final_score(num_dimension, hint + help_count, time);
  cout << "Thank you for playing" << endl;
  return 0;
}

tuple<vector<string>, vector<string>> Expression::pick_operators(int num, int seed)
{
  vector<string> horizontal_ope(num - 1);
  vector<string> vertical_ope(num - 1);
  for (int i = 0; i < num - 1; i++)
  {
    horizontal_ope[i] = operators[rand() % 7]; // pick operators
    vertical_ope[i] = operators[rand() % 7];
  }
  return {horizontal_ope, vertical_ope};
}

int Solution::check_ope(string ope)
{
  if (ope == "*")
    return 1;
  else if (ope == "/")
    return 1;
  else if (ope == "%")
    return 1;
  else if (ope == "+")
    return 2;
  else if (ope == "-")
    return 2;
  else if (ope == "&")
    return 3;
  else if (ope == "^")
    return 4;
  else if (ope == "|")
    return 5;
  else
    return 0;
}

int Solution::calc(int num1, int num2, string ope)
{
  if (ope == "+")
    return num1 + num2;
  else if (ope == "-")
    return num2 - num1;
  else if (ope == "*")
    return num1 * num2;
  else if (ope == "/")
  {
    if (num2 == 0) // avoid the math error
      return 0;
    return num1 / num2;
  }
  else if (ope == "%")
  {
    if (num2 == 0) // avoid the math error
      return 0;
    return num1 % num2;
  }
  else if (ope == "&")
    return num1 & num2;
  else if (ope == "|")
    return num1 | num2;
  else
    return num1 ^ num2;
}

int Solution::row_calc(vector<int> nums, vector<string> ope)
{
  static vector<stack<int>> num_stack(5);
  static vector<stack<string>> ope_stack(5);
  int result;
  bool finish = false;
  for (int i = nums.size() - 1; i >= 0; i--)
    num_stack[0].push(nums[i]);
  for (int i = ope.size() - 1; i >= 0; i--)
    ope_stack[0].push(ope[i]);

  for (int i = 1; i <= 5; i++) // cater for the precedence
  {
    if (finish)
      break;
    while (!num_stack[i - 1].empty())
    {
      if (check_ope(ope_stack[i - 1].top()) != i)
      {
        ope_stack[i].push(ope_stack[i - 1].top());
        ope_stack[i - 1].pop();
        num_stack[i].push(num_stack[i - 1].top());
        num_stack[i - 1].pop();
        if (num_stack[i - 1].size() == 1)
        {
          num_stack[i].push(num_stack[i - 1].top());
          num_stack[i - 1].pop();
        }
      }
      else
      {
        int num1 = num_stack[i - 1].top();
        num_stack[i - 1].pop();
        int num2 = num_stack[i - 1].top();
        num_stack[i - 1].pop();
        num_stack[i - 1].push(calc(num1, num2, ope_stack[i - 1].top()));
        ope_stack[i - 1].pop();
        if (num_stack[i - 1].size() == 1 && ope_stack[i].empty())
        {
          result = num_stack[i - 1].top();
          num_stack[i - 1].pop();
          finish = true;
        }
        else if (num_stack[i - 1].size() == 1 && !ope_stack[i].empty())
        {
          num_stack[i].push(num_stack[i - 1].top());
          num_stack[i - 1].pop();
        }
      }
    }
  }
  return result;
}

tuple<vector<vector<string>>, vector<vector<string>>, vector<vector<int>>> Solution::get_results(Expression &exp, int num, int seed, int min, int max)
{
  srand(seed);
  vector<vector<int>> rand_num(num, vector<int>(num));
  int col = 0;
  int adjust = 0;
  int combination = 1;
  horizontal_ope.resize(num, vector<string>(num - 1));
  vertical_ope.resize(num, vector<string>(num - 1));
  results.resize(2, vector<int>(num));
  initial_nums.resize(num, vector<int>(num));
  while (col < num)
  {
    for (int i = 0; i < num; i++)
    {
      rand_num[col][i] = rand() % (max - min + 1) + min;
    }
    auto [row_ope, col_ope] = exp.pick_operators(num, seed + adjust);
    results[0][col] = row_calc(rand_num[col], row_ope);
    int v_count = row_solutions(num, 1, 9, col, results[0][col], row_ope);
    if (v_count > 150 || v_count == 0) // if all possible solution is more than 150
    {
      continue;
      adjust++;
      srand(seed + adjust); // reset the random num seed
    }
    cout << v_count << " possible solutions in row " << col << endl;
    horizontal_ope[col] = row_ope;
    vertical_ope[col] = col_ope;
    col++;
    adjust++;
    combination *= v_count;
    srand(seed + adjust);
  }
  for (int i = 0; i < num; i++) // calculate results for collumns
  {
    vector<int> nums_col;
    for (int j = 0; j < num; j++)
    {
      nums_col.push_back(rand_num[j][i]);
    }
    results[1][i] = row_calc(nums_col, vertical_ope[i]);
  }
  initial_nums = rand_num;
  if (combination < 10000) // if the combination of row solutions is less than 10000
    col_calc(num, vrows_count, solutions);
  else
    cout << "\nSolutions still possible\nSolutions are too many, play more to get the number of solutions" << endl;
  return {horizontal_ope, vertical_ope, results};
}

int Solution::row_solutions(int num, int min, int max, int col, int result, vector<string> row_ope)
{
  vrows_count.resize(num, 0);
  solutions.resize(num, vector<vector<int>>(100000, vector<int>(num)));
  vector<int> end(num, max);
  end[0]++;
  vector<int> tmp(num, min);
  vector<int> vrow_counter(num, 0);
  tmp[0]--;
  int count = 0;
  int count1 = 0;
  vrows_count[col] = 0;
  while (1) // going through all permutations
  {
    tmp[0]++;
    if (end == tmp)
      break;
    for (int i = 0; i < num; i++)
    {
      if (tmp[i] == max + 1)
      {
        tmp[i] = min;
        tmp[i + 1]++;
      }
    }
    int tmp_result = row_calc(tmp, row_ope);
    if (tmp_result == result)
    {
      solutions[col][count] = tmp; // store the possible solution
      vrows_count[col]++;
      count++;
    }
  }
  return count;
}

tuple<int, vector<vector<int>>> Solution::check_num_solutions(int num_col, int row, vector<vector<string>> board, vector<vector<bool>> valid_cell)
{
  int count = 0;
  vector<vector<int>> valid_solutions(150, vector<int>(num_col));
  for (int k = 0; k < vrows_count[row]; k++)
  {
    bool valid = true;
    for (int i = 0; i < num_col; i++)
    {
      if (valid_cell[row][i])
      {
        if (board[row * 2][i * 2] == to_string(solutions[row][k][i]))
        {
          valid = true;
        }
        else
        {
          valid = false;
          break;
        }
      }
    }
    if (valid)
    {
      valid_solutions[count] = solutions[row][k]; // store the possible solution
      count++;
    }
  }
  return {count, valid_solutions};
}

int Solution::col_calc(int num, vector<int> count, vector<vector<vector<int>>> v_solutions)
{
  int n_count = 0;
  vector<vector<int>> tmp_board(num, vector<int>(num));
  vector<int> current_counter(num, 0);
  while (current_counter[num - 1] != count[num - 1] + 1) // going through all permutation within the valid solutions
  {
    for (int i = 0; i < num; i++)
      tmp_board[i] = v_solutions[i][current_counter[i]]; // combine the possible solutions from each row
    bool valid = true;
    for (int i = 0; i < num; i++)
    {
      vector<int> nums(num);
      for (int j = 0; j < num; j++)
        nums[j] = tmp_board[j][i];
      vector<string> ope = vertical_ope[i];
      int res = row_calc(nums, ope);
      if (res == results[1][i])
        valid = true;
      else
      {
        valid = false;
        break;
      }
    }
    if (valid)
    {
      n_count++;
    }
    current_counter[0]++;
    for (int i = 0; i < num; i++)
    {
      if (current_counter[i] == count[i])
      {
        if (i != num - 1)
        {
          current_counter[i] = 0;
          current_counter[i + 1]++;
        }
        else
        {
          current_counter[i]++;
          break;
        }
      }
    }
    if (current_counter[num - 1] == count[num - 1] + 1)
      break;
  }
  return n_count;
}

Board::Board(int num, int min_range, int max_range, int seed) : solution()
{
  num_col = num;
  min = min_range;
  max = max_range;
  seed_num = seed;
  status = false;
  data_type = 0;
  valid_cell.resize(num_col, vector<bool>(num_col, false));
  board.resize(num_col * 2 + 1, vector<string>(num_col * 2 + 1));

  auto [horizontal_ope, vertical_ope, results] = solution.get_results(expression, num_col, seed_num, min_range, max_range);

  for (int i = 0; i < num_col; i++) // initialize the board
  {
    for (int j = 0, k = 0; j < num_col; j++)
    {
      board[i * 2][k] = "#";
      k++;
      if (j == num_col - 1)
      {
        board[i * 2][k] = "=";
        board[i * 2][k + 1] = to_string(results[0][i]);
        continue;
      }
      board[i * 2][k] = horizontal_ope[i][j];
      k++;
    }
    if (i == num_col - 1)
    {
      continue;
    }
    for (int j = 0; j < num_col; j++)
    {
      board[i * 2 + 1][j * 2] = vertical_ope[j][i];
      board[i * 2 + 1][j * 2 + 1] = " ";
    }
  }
  for (int i = 0; i < num_col * 2; i++)
  {
    if (i % 2 == 0)
    {
      board[num_col * 2][i] = to_string(results[1][i / 2]);
    }
    else
    {
      board[num_col * 2][i] = " ";
    }
  }
}

bool Board::update(int row, int collumn, int value)
{
  bool check_end = false;
  int combination = 1;
  int possible = 0;
  vector<int> v_rows_count(num_col);
  vector<vector<vector<int>>> current_valid_solutions(num_col, vector<vector<int>>(1000, vector<int>(num_col)));
  if (value > 0)
  {
    board[(row - 1) * 2][(collumn - 1) * 2] = to_string(value); // update the board
    valid_cell[row - 1][collumn - 1] = true;                    // fill the cell
  }
  else
  {
    board[(row - 1) * 2][(collumn - 1) * 2] = "#"; // undo it if value is 0
    valid_cell[row - 1][collumn - 1] = false;
  }
  for (int i = 0; i < num_col; i++)
  {
    auto [tmp_count, tmp_solutions] = solution.check_num_solutions(num_col, i, board, valid_cell); // check the current possible solutions
    cout << tmp_count << " possible solutions found in row " << i << endl;
    v_rows_count[i] = tmp_count;
    current_valid_solutions[i] = tmp_solutions;
    combination *= tmp_count;
  }
  cout << endl;
  for (auto s : v_rows_count)
    if (s == 0)
    {
      cout << "Solution impossible" << endl;
      return false;
    }
  if (combination < 10000) // if the combination is not too many
  {
    possible = solution.col_calc(num_col, v_rows_count, current_valid_solutions);
    if (possible == 0)
      cout << "Solution impossible" << endl;
    else
      cout << possible << " possible solutions exist in this board." << endl;
  }
  else
    cout << "Solutions still possible\nSolutions are too many, play more to get the number of solutions" << endl;
  for (int i = 0; i < num_col; i++)
  {
    for (int j = 0; j < num_col; j++)
    {
      if (valid_cell[i][j])
        check_end = true;
      else
      {
        check_end = false;
        break;
      }
    }
    if (check_end == false)
      break;
  }
  if (check_end && possible == 1) // check if the game is ended
    return true;
  else
    return false;
}

void Board::initial_hint(int num_hint)
{
  int adjust = 0;
  if (num_hint > num_col * num_col)
    num_hint = num_col * num_col;
  int row = 0;
  for (int i = 0; i < num_hint; i++)
  {
    srand(adjust++);
    int index = rand() % (num_col);
    if (valid_cell[row][index])
    {
      i--;
      continue;
    }
    else
    {
      board[(row)*2][(index)*2] = to_string(solution.initial_nums[row][index]);
      valid_cell[row][index] = true;
      if (i == num_hint - 1)
      {
        update(row + 1, index + 1, solution.initial_nums[row][index]); // pull over from the initial numbers
      }
      if (row >= num_col - 1)
        row = 0;
      else
        row++;
    }
  }
}

void Board::help_next()
{
  int adjust = 0;
  int combination = 1;
  int possible = 0;
  vector<int> v_rows_count(num_col);
  vector<vector<vector<int>>> current_valid_solutions(num_col, vector<vector<int>>(150, vector<int>(num_col)));
  for (int i = 0; i < num_col; i++) // check the number of combinations
  {
    auto [tmp_count, tmp_solutions] = solution.check_num_solutions(num_col, i, board, valid_cell);
    v_rows_count[i] = tmp_count;
    current_valid_solutions[i] = tmp_solutions;
    combination *= tmp_count;
  }
  if (combination < 10000)
  {
    possible = solution.col_calc(num_col, v_rows_count, current_valid_solutions);
    if (possible == 0) // if the solution impossible
    {
      cout << "**It's impossible to find a next valid number from this board" << endl;
      return;
    }
  }
  bool finish = false;
  int cell = 0;
  srand(seed_num);
  for (int row = 0; row < num_col; row++) // go through all cells
  {
    if (finish)
      break;
    for (int index = 0; index < num_col; index++)
    {
      if (valid_cell[row][index])
      {
        continue;
      }

      board[row * 2][index * 2] = to_string(current_valid_solutions[row][cell][index]);
      valid_cell[row][index] = true;
      combination = 1;
      possible = 0;
      vector<int> v_rows_count_tmp(num_col);
      vector<vector<vector<int>>> current_valid_solutions_tmp(num_col, vector<vector<int>>(150, vector<int>(num_col)));
      for (int i = 0; i < num_col; i++)
      {
        auto [tmp_count, tmp_solutions] = solution.check_num_solutions(num_col, i, board, valid_cell);
        v_rows_count_tmp[i] = tmp_count;
        current_valid_solutions_tmp[i] = tmp_solutions;
        combination *= tmp_count;
      }
      if (combination < 10000)
      {
        possible = solution.col_calc(num_col, v_rows_count_tmp, current_valid_solutions_tmp);
        if (possible == 0)
        {
          finish = false;
          board[row * 2][index * 2] = "#";
          valid_cell[row][index] = false;
          index--;
          cell++;
          continue;
        }
        else
        {
          for (int i = 0; i < num_col; i++)
            cout << v_rows_count_tmp[i] << " possible solutions found in row " << i << endl;
          finish = true;
          cout << possible << " possible solutions exist in this board." << endl;
          break;
        }
      }
      else
      {
        for (int i = 0; i < num_col; i++)
          cout << v_rows_count_tmp[i] << " possible solutions found in row " << i << endl;
        finish = true;
        cout << possible << " possible solutions exist in this board." << endl;
        break;
      }
    }
  }
}

void Board::final_score(int num, int help, int time)
{
  int score = num * (100 - help) * (5000 - time); // calculate the score
  cout << "Your score is " << score << endl;
}

ostream &operator<<(ostream &os, const Board &b)
{
  os << endl;
  for (int i = 0; i < b.num_col; i++)
    os << setw(3) << i + 1 << " ";
  os << endl;
  for (int i = 0; i < b.num_col * 2 - 1; i++) // print the board
  {
    if (i % 2 == 0)
    {
      os << i / 2 + 1 << " ";
      if (b.data_type == 0) // if decimal
      {
        for (int j = 0; j < b.num_col * 2; j++)
        {
          if (isdigit(b.board[i][j][0]))
          {
            cout << dec << stoi(b.board[i][j]) << " ";
          }
          else
          {
            os << b.board[i][j] << " ";
          }
        }
        os << b.board[i][b.num_col * 2] << endl;
      }
      else if (b.data_type == 1) // if binary
      {
        for (int j = 0; j < b.num_col * 2; j++)
        {
          if (isdigit(b.board[i][j][0]))
          {
            bitset val = bitset<8>(stoi(b.board[i][j]));
            cout << val << " ";
          }
          else
          {
            os << b.board[i][j] << " ";
          }
        }
        os << b.board[i][b.num_col * 2] << endl;
      }
      else if (b.data_type == 2) // if octal
      {
        for (int j = 0; j < b.num_col * 2; j++)
        {
          if (isdigit(b.board[i][j][0]))
          {
            cout << oct << stoi(b.board[i][j]) << " ";
          }
          else
          {
            os << b.board[i][j] << " ";
          }
        }
        os << b.board[i][b.num_col * 2] << endl;
      }
      else // if hex
      {
        for (int j = 0; j < b.num_col * 2; j++)
        {
          if (isdigit(b.board[i][j][0]))
          {
            cout << hex << stoi(b.board[i][j]) << " ";
          }
          else
          {
            os << b.board[i][j] << " ";
          }
        }
        os << b.board[i][b.num_col * 2] << endl;
      }
    }
    else
    {
      os << " " << setw(2);
      for (int j = 0; j < b.num_col * 2; j++)
        os << b.board[i][j] << " ";
      os << endl;
    }
  }
  os << endl
     << " " << setw(2);
  for (int i = 0; i < b.num_col * 2; i++)
  {
    os << b.board[b.num_col * 2][i] << " ";
  }
  os << endl
     << endl;
  if (b.data_type == 0)
    os << "Current display is decimal" << endl;
  else if (b.data_type == 1)
    os << "Current display is binary" << endl;
  else if (b.data_type == 2)
    os << "Current display is octal" << endl;
  else
    os << endl
       << "Current display is hex decimal" << endl;
  os << endl
     << "***Commnad list***\n<[row] [collumn] [insert number]> insert number in the board\n<decimal> display board in decimal\n<binary> display board in binary\n<octal> display board in octal\n<hex> display board in hex decimal\n<help> insert the next valid number in the board\n<quit> exit the game\n";

  return os;
}
