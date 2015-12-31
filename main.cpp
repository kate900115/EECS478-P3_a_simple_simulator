#include "circuit.h"
#include <math.h>

#include <utility>

using namespace std;

void usage(const char* exename);

int main(int argc, char **argv)
{
  // parsing inputs
  if (argc < 2)
  {
    usage(argv[0]);
  }
  
  for (int i = 1; i < argc; ++i)
  {
    if (argv[i] == string("-h") || argv[i] == string("-help"))
    {
      usage(argv[0]);
    }
    
    else if (argv[i] == string("-topoSort"))
    {
      if (i + 1 < argc)
      {
        string inFilename = string(argv[++i]);
        Circuit c(inFilename);
        
        // your code here
        
        // setup the input list and the output list
        vector<Node*> inputs_list = c.getPIs();
        vector<Node*> outputs_list;
        
        // setup the node map, which should include all the node of this circuit
        map<string,Node*> nodemap = c.getnodeMap();
        
        // the input list should include the primary input and the zero nodes and one nodes
        for (mapIter it = nodemap.begin(); it != nodemap.end(); it++)
        {
        	if ((it->second->getType()==ZERO_NODE)||(it->second->getType()==ONE_NODE))
        	{
        		inputs_list.push_back(it->second);
        	}
        }
        
        
        while (inputs_list.size()!=0)
        {
        	Node* Node_new = inputs_list.front();
        	inputs_list.erase(inputs_list.begin()+0);
        	//cout<<"print Node_new"<<endl; 
        	//Node_new->print();
        	
        	for (mapIter it = nodemap.begin(); it != nodemap.end(); it++) 
  			{
  				//cout<<"print it"<<endl;
  				//it->second->print();
			    if ((it->second->getType() == INTERNAL)||(it->second->getType() == PRIMARY_OUTPUT))
    			{
    				vector<Node*> fan_in = it->second->getFanin();
				 	int fan_in_size = fan_in.size();
				 	for(int i=0; i < fan_in_size; i++)
    				{
    				 	Node* node_fanin = fan_in.front();
    				 	//cout<<"print node_fanin"<<endl;
						//node_fanin->print();
    				 	it->second->eraseFanin(node_fanin);
    				 	if (node_fanin != Node_new)
    				 	{
    				 		fan_in.push_back(node_fanin);
    				 	}
    				 }
    				 if(fan_in.size()==0)
    				 {
    				 	c.setPI(it->second->getName());
						inputs_list.push_back(it->second);
    				 }
    			}
    		}
    		outputs_list.push_back(Node_new);
        }
   
   		// print out the topological order:
  		cout<<"*** Topological order:"<<endl;
  		
    	while (outputs_list.size()!=0)
    	{
    		Node* OutNode = outputs_list.front();
    		outputs_list.erase(outputs_list.begin()+0);
    		cout << OutNode->getName()<<" ";
    	}
  		cout << endl;
      }
      else
      {
        cout << "option -topoSort requires an additional argument." << endl;
        usage(argv[0]);
      }
    }
    else if (argv[i] == string("-simulate"))
    {
      if (i + 2 < argc)
      {
        string inFilename = string(argv[++i]);
        Circuit c(inFilename);
        string inputFile(argv[++i]);
        // your code here
        
        // set the primary input into one node and zero node
        c.readINPUT(inputFile);
        map<string,Node*> nodemap = c.getnodeMap();
        
        vector<Node*> one_zero_list;
        vector<Node*> outputs_list;
        
        // initialize the one_zero_list and output_list
        for (mapIter it = nodemap.begin(); it != nodemap.end(); it++) 
        {
        	if ((it->second->getType() == ONE_NODE)||(it->second->getType() == ZERO_NODE))
        	{
        		one_zero_list.push_back(it->second);
        		//it->second->print();
        	}
        	else if(it->second->getType() == PRIMARY_OUTPUT)
    		{
    			outputs_list.push_back(it->second);
    			//it->second->print();
    		}
        }
        
        while (c.getNumNodes() != one_zero_list.size())
        {
        	// traverse all the internal node and output node
        	for (mapIter it = nodemap.begin(); it != nodemap.end(); it++) 
  			{
  				if ((it->second->getType() == INTERNAL)||(it->second->getType() == PRIMARY_OUTPUT))
    			{
    				vector<Node*> fan_in = it->second->getFanin();
    				int fan_in_size = fan_in.size();
    			
    				vector<truthType> fanin_logic;
    				
    				for (int i=0; i<fan_in_size; i++)
    				{
    					Node* faninNode = fan_in.front();
    					fan_in.erase(fan_in.begin()+0);
    					if (faninNode->getType()==ZERO_NODE)
    					{
    						fanin_logic.push_back(ZERO);
    					} 
    					else if (faninNode->getType()==ONE_NODE)
    					{
    						fanin_logic.push_back(ONE);
    					}
    					fan_in.push_back(faninNode);
    				}
    				
    				// if the fan in of current node are all one/zero nodes,
        			// use the truthtable of this node to calculate the value of this node
    				// calculate the truthtable of fanin
    				
    				if (fan_in.size()==fanin_logic.size())
    				{
    					for (unsigned i=0; i<it->second->getTT().getNumVars(); i++)
    					{
    						if (fanin_logic==it->second->getTT().getTruthTable()[i])
    						{
    							it->second->setType(ONE_NODE);
    							break;
    							//it->second->print();
    						}
							else
							{
								it->second->setType(ZERO_NODE);	
								//it->second->print();
							}
    					}
    					one_zero_list.push_back(it->second);
    				}
    			}	
    		}
  		}
  		
  		// print the node and its value ot the output list
  		while (outputs_list.size()!=0)
  		{
  			Node* outNode = outputs_list.front();
  			outputs_list.erase(outputs_list.begin()+0);
  			//outNode->print();
  			if (outNode->getType() == ZERO_NODE)
  			{
  				if (outputs_list.size()==0)
  				{
  					cout << outNode->getName()<<"=0"; 
  				}
  				else
  				{
  					cout << outNode->getName()<<"=0, "; 
  				}
  			}
  			else if (outNode->getType() == ONE_NODE)
  			{
  				if (outputs_list.size()==0)
  				{
  					cout << outNode->getName()<<"=1"; 
  				}
  				else
  				{
  					cout << outNode->getName()<<"=1, "; 
  				}
  			}
  		} 
  		cout<<endl;
        
        // if the fan in of current node are all one/zero nodes,
        // use the truthtable of this node to calculate the value of this node
        // change the type from internal/output into one/zero
        
      }
      else
      {
        cout << "option -simulate requires two additional arguments." << endl;
        usage(argv[0]);
      }
    }
    
  }
  
  return 0;
}

void usage(const char* exename)
{
  cout << "Usage: " << exename << " <options> " << endl;
  cout << "-h or -help                     prints out this help message. " << endl;
  cout << "-topoSort <inFile>              prints a topological ordering of the circuit in <inFile>. " << endl;
  cout << "-simulate <inFile> <inputs>     simulates the circuit in <inFile> with the inputs in <inputs>." << endl;
  cout << endl;
  
  exit(0);
}

