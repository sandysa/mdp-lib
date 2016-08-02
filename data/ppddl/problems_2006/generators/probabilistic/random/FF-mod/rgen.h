#define LEN_ACTION_NAME 4
#define NUM_ACTIONS 100

typedef struct
{
  char name[LEN_ACTION_NAME];
  int num_prob_effects;
  int *probs;
  char **prob_name;
} action_table_type;

