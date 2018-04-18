/*
 * Lexial_Analyzer.cpp
 * Created on: Mar 23, 2018
 * Author: Amr Hendy
 */

#include "Lexical_Analyzer.h"

/* CONSTRUCTOR */
/*********************************************/
Lexical_Analyzer::Lexical_Analyzer(TransitionTable minimized_dfa_table, string user_program_directory)
{
    /* read user file */
    user_program = FileHandler::readUserProgram(user_program_directory);
    /* initialize Lexial_Analyzer attributes */
    Lexical_Analyzer::minimized_dfa_table = minimized_dfa_table;
    current_index = 0;
}


Lexical_Analyzer::Lexical_Analyzer()
{

}

Lexical_Analyzer::~Lexical_Analyzer()
{
    // TODO Auto-generated destructor stub
}


/* INTERFACE FUNCTIONS */
/*********************************************/
Token
Lexical_Analyzer::next_token()
{
    /* initialize helpers */
    CompositeState* current_state = minimized_dfa_table.get_start_state();
    CompositeState* last_acceptance_state;
    int last_acceptance_index = -1;
    int start_index = current_index ;

    while(current_index < (int) user_program.size())
    {
        /* read new char into buffer */
        char current_char = user_program.at(current_index);

        /* transition in table according to input char */
        current_state = minimized_dfa_table.toke_get_transition(current_state, current_char);
        /* check current state */

        if(current_state->get_size() == 0)
        {
            if(last_acceptance_index != -1)
            {
                string lexeme;
                lexeme = user_program.substr(start_index, last_acceptance_index - start_index + 1);
                current_index = last_acceptance_index + 1;
                vector<Rule> conflicting_rules = last_acceptance_state->get_rules();
                return get_correct_token(conflicting_rules, lexeme);
            }
            else
            {
                current_index = start_index + 1;
                if(current_char != ' ')
                    cout << "'" << user_program[start_index] << "'" << "\tError don not match any rules"<<  endl;
                return next_token();
            }
        }

        if(current_state->is_acceptance())
        {
            last_acceptance_state = current_state;
            last_acceptance_index = current_index;
        }
        current_index++;
    }


    if(last_acceptance_index != -1)
    {
        string lexeme;
        lexeme = user_program.substr(start_index, last_acceptance_index - start_index + 1);
        current_index = last_acceptance_index + 1;
        vector<Rule> conflicting_rules = last_acceptance_state->get_rules();
        last_acceptance_index = -1;
        return get_correct_token(conflicting_rules, lexeme);
    }
    else if(start_index == (int) user_program.size() )
    {
        return Token("INVALID","INVALID","INVALID");
    }
    else
    {
        current_index = start_index + 1;
        //report erros
        return next_token();
    }
}

Token
Lexical_Analyzer::get_correct_token(vector<Rule> conflicting_rules, string lexeme)
{
    Token result = Token("", "", "");
    int max_priority = 999 ;
    for(Rule rule : conflicting_rules)
    {
        if(rule.getPriority() < max_priority)
        {
            max_priority = rule.getPriority();
            result = Token(rule.getRuleName(), rule.getRulePattern(), lexeme);
        }
    }
    return result;
}
