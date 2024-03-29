#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>


void set_random_seed();
int randn(int n);

struct mm_code_maker{
  void init(int i_length, int i_num){
    length = i_length;
    num = i_num;
  }
  void generate_sequence(){
    for(int i = 0; i < length; i++){
      sequence.push_back(randn(num));
    }
  }
  //moje dodane
  void generate_sequence(int i){
    std::vector<int> temp(length);
    for(int place = length - 1; place >= 0; place--){
      temp[place] = i % num;
      i = i / num;
    }
    sequence = temp;
  }

  void give_feedback(const std::vector<int>& attempt, int& black_hits, int& white_hits){
    black_hits = 0;
    white_hits = 0;
    int* numFreqAttempt = new int[num];
    int* numFreqSequence = new int[num];

    for(int i = 0; i < num; i++){
      numFreqAttempt[i] = 0;
      numFreqSequence[i] = 0;
    }

    for(int j = 0; j < length; j++){
      numFreqAttempt[attempt[j]]++;
      numFreqSequence[sequence[j]]++;
      if(attempt[j] == sequence[j]){
        black_hits++;
      }
    }
    int hits = 0;
    for(int k = 0; k < num; k++) {
      if(numFreqAttempt[k]>numFreqSequence[k]){
        hits += numFreqSequence[k];
      }else{
        hits += numFreqAttempt[k];
      }
    }
    white_hits = hits - black_hits;
    delete numFreqAttempt;
    delete numFreqSequence;
  }

  std::vector<int> sequence;
  int length;
  int num;
};

struct mm_solver{
  void init(int i_length, int i_num){
    length = i_length;
    num = i_num;
    first = true;
    std::vector<int> temp(length,0);
    for(int i = 0; i < pow(num,length); i++){
      possible.push_back(temp);
      attempt_sequential(temp);
    }
    max =  num * length * pow(num, length) / 2;
    no_of_partitions = length * (length + 3) /2;
    //DEBUGING
    std::vector<int> v(100,0);
    depth = v;
    id = 0;
  }

  void create_attempt(std::vector<int>& attempt){
    if(first){
      std::vector<int> temp(length,0);
      for(int i = 0; i < length - 1; i++){
        if(i < num){
          temp[i+1] = i;
        }
      }
      attempt = temp;
      first = false;
    }else if(possible.size() == 1){
      attempt = possible[0];
    }else if(possible.size() == 2){
      attempt = possible[0];
    }else{
      long int external_path_length = 0;
      min_external_path_length(external_path_length, possible, past_guesses, attempt);
      std::cout << "average: " << external_path_length / pow(num,length) << '\n';
      std::cout << "epl: " << external_path_length << '\n';
      if(external_path_length == -1){
        std::cout << "CIRTICAL ERROR" << '\n';
      }
    }
  }

  void learn(std::vector<int>& attempt, int black_hits, int white_hits){
    past_guesses.push_back(attempt);
    std::vector< std::vector<int> > temp;
    int b_hits;
    int w_hits;
    for(int i = 0; i < possible.size(); i++){
      give_feedback(attempt, possible[i], b_hits, w_hits);
      if((b_hits == black_hits) && (w_hits == white_hits)){
        temp.push_back(possible[i]);
      }
    }
    possible = temp;
  }

  int length;
  int num;
  std::vector< std::vector<int> > possible;
  std::vector< std::vector<int> > past_guesses;
  bool first;
  long int max;
  int no_of_partitions;
  //def
  std::vector<int> depth;
  int id;

  void min_external_path_length(long int& external_path_length, std::vector<std::vector <int> >& t_possible, std::vector<std::vector<int> >& attempted, std::vector<int>& attempt){
    //std::cout << "depth: " << id << " count: " << depth[id] << '\n';
    external_path_length += t_possible.size();
    if(external_path_length >= max){
      external_path_length = -1;
    }else if(t_possible.size() <= 2){
      if(t_possible.size()==1){
        attempt = t_possible[0];
      }else{
        attempt = t_possible[0];
      }
    }else{
      std::vector< std::vector< std::vector< std::vector <int> > > > partition_of_possible;
      initalize_partion(partition_of_possible);
      std::vector<int> current_attempt(length , 0);
      std::vector<int> epl_all;

      for(int code = 0; code < pow(num,length); code++){
        bool skip = false;
        for(int i = 0; (i < attempted.size()) && (!skip); i++){
          if(code == create_number_from_attempt(attempted[i])) skip = true;
        }
        if(skip){
          epl_all.push_back(-1);
        }else {
          long int epl = external_path_length;
          empty_partiton(partition_of_possible);
          fill_partiton(current_attempt, t_possible, partition_of_possible);
          //remeber previous guesses
          std::vector< std::vector<int> > curr_past_attempts = attempted;
          curr_past_attempts.push_back(current_attempt);
          //calculate the sum min_external_path_length for all partions
          bool go_on = true;
          for(int row = 0; (row < length)&&go_on; row++){
            for(int col = 0; (col <= length - row) && go_on; col++){
              if(!(partition_of_possible[row][col].empty())){
                //id++;
                //depth[id]++;
                min_external_path_length(epl, partition_of_possible[row][col], curr_past_attempts, current_attempt);
                //id--;
                if(epl == -1){
                  go_on = false;
                }
              }
            }
          }
          epl_all.push_back(epl);
          if((epl < max) && (epl != -1)){
            max = epl;
            std::cout << "new max: " << max << '\n';
          }
        } //end of if
        attempt_sequential(current_attempt);
      } //end of for
      //find minimum in min_external_path_length;
      int index_min = -1;
      int min = -1;
      bool found = false;
      for(int i = 0; i < pow(num,length); i++){
        if(!found && epl_all[i] != -1){
          min = epl_all[i];
          index_min = i;
          found = true;
        }else{
          if((epl_all[i] < min) && (epl_all[i] != -1)){
            min = epl_all[i];
            index_min = i;
          }
        }
      }
      //finding if there is min in possible space
      external_path_length = min;
      found = false;
      for(int i = 0; (i < possible.size()) && (!found); i++) {
        int temp_index = create_number_from_attempt(possible[i]);
        if(epl_all[temp_index] == min){
          index_min = temp_index;
          found = true;
        }
      }
      create_attempt_from_number(attempt, index_min);
    }
    //std::cout << "epl: " << external_path_length << '\n';
  }

  void initalize_partion(std::vector< std::vector< std::vector< std::vector <int> > > >& partition_of_possible){
    for(int row = 0; row <= length; row++){
      std::vector< std::vector< std::vector<int> > > temp1;
      partition_of_possible.push_back(temp1);
        for(int col = 0; col <= length - row; col++){
          std::vector< std::vector<int> > temp2;
          partition_of_possible[row].push_back(temp2);
        }
    }
  }
  void empty_partiton(std::vector< std::vector< std::vector< std::vector <int> > > >& partition_of_possible){
    std::vector<std::vector<int> > temp;
    for(int row = 0; row <= length; row++){
      for(int col = 0; col <= length - row; col++){
        partition_of_possible[row][col].clear();
      }
    }
  }
  void fill_partiton(std::vector<int>& attempt, std::vector<std::vector<int> >& possible,
                    std::vector< std::vector< std::vector< std::vector <int> > > >& partition_of_possible){
    int black_hits;
    int white_hits;
    for(int i = 0; i < possible.size(); i++){
      give_feedback(attempt, possible[i], black_hits, white_hits);
      partition_of_possible[black_hits][white_hits].push_back(possible[i]);
    }
  }

  void attempt_sequential(std::vector<int>& current_attempt){
    bool carry = true;
    for(int place = length - 1; (place >= 0) && carry; place--){
      carry = false;
      current_attempt[place]++;
      if(current_attempt[place] == num){
        current_attempt[place] = 0;
        carry = true;
      }
    }
  }
  void give_feedback(const std::vector<int>& attempt, const std::vector<int>& prev_possible,
                   int& black_hits, int& white_hits){
    black_hits = 0;
    //std::vector<int> numFreqAttempt(num,0);
    //std::vector<int> numFreqSequence(num,0);
    int* numFreqAttempt = new int[num];
    int* numFreqSequence = new int[num];
    for(int i = 0; i < num; i++){
      numFreqAttempt[i] = 0;
      numFreqSequence[i] = 0;
    }

    for(int j = 0; j < length; j++){
      numFreqAttempt[attempt[j]]++;
      numFreqSequence[prev_possible[j]]++;
      if(attempt[j] == prev_possible[j]){
        black_hits++;
      }
    }
    int hits = 0;
    for(int k = 0; k < num; k++) {
      if(numFreqAttempt[k]>numFreqSequence[k]){
        hits += numFreqSequence[k];
      }else{
        hits += numFreqAttempt[k];
      }
    }
    white_hits = hits - black_hits;
    delete numFreqAttempt;
    delete numFreqSequence;
  }
  void create_attempt_from_number(std::vector<int>& attempt, int index){
    std::vector<int> temp(length);
    for(int place = length - 1; place >= 0; place--){
      temp[place] = index % num;
      index = index / num;
    }
    attempt = temp;
  }
  int create_number_from_attempt(std::vector<int> attempt){
    int sum = attempt[0];
    for(int place = 1; place < length; place++){
      sum *= num;
      sum += attempt[place];
    }
    return sum;
  }
  //DEBUGING
  void print_partion(std::vector< std::vector< std::vector< std::vector <int> > > >& partition_of_possible){
    for(int row = 0; row <= length; row++){
      for(int col = 0; col <= length - row; col++){
          std::cout << "row: " << row << " col: " << col << '\t';
          std::vector<std::vector<int> > v = partition_of_possible[row][col];
          for(int j = 0; j < v.size(); j++){
              for(int i = 0; i < length; i++){
                std::cout << v[j][i];
              }
              std::cout << " ";
            }
          }
        }
  }
  void print_v_v(std::vector<std::vector<int> > v){
    std::cout << "size: "<< v.size() << "  length: " << v[0].size() << '\n';
    for(int i = 0; i < v.size(); i++){
      for(int j = 0; j < v[i].size(); j++){
        std::cout << v[i][j];
      }
      std::cout << '\t';
    }
  }
};

int main(){
  set_random_seed();
  int length, num;
  std::cout << "enter length of sequence and number of possible values:" << std::endl;
  std::cin >> length >> num;
  std::vector<int> attemptsN;
  std::vector<float> times;
  for (int i = 0; i < pow(num,length); i++) {
    mm_solver solver;
    solver.init(length, num);
    mm_code_maker maker;
    maker.init(length, num);
    maker.generate_sequence(i);

    int black_hits = 0, white_hits=0;
    int attempts_limit = 10;
    int attempts = 0;

    clock_t start = clock();
    while((black_hits < length) && (attempts < attempts_limit)){
      std::vector<int> attempt;
      solver.create_attempt(attempt);
      maker.give_feedback(attempt, black_hits, white_hits);
      solver.learn(attempt, black_hits, white_hits);
      attempts++;
    }
    clock_t end = clock();

    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    std::cout << '\n';
    std::cout << "time: " << seconds << '\n';
    if(black_hits == length){
      std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
    }
    else{
      std::cout << "after " << attempts << " attempts still no solution" << std::endl;
    }
    std::cout << "the sequence generated by the code maker was:" << std::endl;
    for(int i = 0; i < maker.sequence.size(); i++){
      std::cout << maker.sequence[i] << " ";
    }
    std::cout << std::endl;

    attemptsN.push_back(attempts);
    times.push_back(seconds);
  }

  int sumN = 0;
  int maxN = attemptsN[0];
  float sumT = 0.0;
  for(int i = 0; i < attemptsN.size(); i++){
    sumN += attemptsN[i];
    if(attemptsN[i]>maxN) maxN = attemptsN[i];
    sumT += times[i];
  }
  float averageN = float(sumN) / attemptsN.size();
  float averageT = sumT / attemptsN.size();
  std::cout << "Average guesses: " << averageN << '\n';
  std::cout << "Maximal guesses: " << maxN << '\n';
  std::cout << "Average time: " << averageT << '\n' << '\n';

  std::vector<int> guessFreq(maxN+1);

  for(int i = 0; i < maxN; i++) guessFreq[i] = 0;
  for(int i = 0; i < attemptsN.size(); i++) guessFreq[attemptsN[i]]++;

  std::cout << "Guess frequency" << '\n';
  for(int i = 1; i < guessFreq.size(); i++) {
    std::cout << i << ". " << guessFreq[i] << '\n';
  }
  return 0;
}

void set_random_seed(){
    std::srand(std::time(0));
}

int randn(int n){
    return std::rand() % n;
}
