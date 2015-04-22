#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>

#define ALPHABET 26
#define INF 10000
#define PACKET_SIZE 10

// Node for the Huffmann tree
typedef struct Node_ Node;
struct Node_ {
    int ch;
    double p;
    Node* left;
    Node* right;
    Node* parent;
    char* code ;
};

char** codeTable;
// To get the node to be added each time to the hoffmann tree.
int getMinIndex(double pb[]) {
	int i = 0;
	double min = 1000;
	int minIndex = -1;
	for (i = 0; i < ALPHABET ; i++){
		if (min > pb[i]){
			min = pb[i];
			minIndex = i;
		}
	}
	return minIndex;
}
// Define a new node's attributes.
Node* newNode(double p,int ch){
	Node* node=(Node*)malloc(sizeof(Node));
	node->p=p;
	node->ch = ch;
	node->left=NULL;
	node->right=NULL;
	node->code = (char*)malloc(sizeof(char)*100);
	return (node);
}

// Code that makes a Huffmann tree and returns its root.
Node*  makeHuffmannTree(double pb[]) {
	int i;
	// Get the minIndex node .
	int minIndex = getMinIndex (pb);

	// Starting from the node with least probability.
	Node* root   = newNode (pb[minIndex],minIndex);
	root->code = NULL;
	pb[minIndex] = 1000;
	// Get the next least node.
	int k = getMinIndex(pb);
	while (k!=-1) {
		// Now temp and node are the two nodes to be added to the Huffmann tree.
		Node* node = newNode(pb[k],k);
		Node* temp = root;
		// root is the parent of node and temp.
		// Go on building the Huffmann tree as long as there are independent nodes.
		root = newNode ((node->p) + (root->p),1000);
		if (node->p <= temp->p){	
			root->left  = node ;
			root->right = temp ; 
			node->parent= root ;
			temp->parent= root ;
			
		}
		else{
			root->right = node ;
			root->left  = temp ;
			node->parent= root ;
			temp->parent= root ;
		}
		root->ch = INF;
		pb[k] = 1000;
		k = getMinIndex(pb);
		
	}
		root->code = NULL;
		return root;
	
}






// Searches for an alphabet in the Huffmann tree and returns its code.

char* getCode (Node* root, double p,int k){
	Node* temp = root ;

	// Initialize to the null string.

	char* s = (char*)malloc(sizeof(char)*100);
	int i = 0;
	while (i == 0) {

		// When right node is not an alphabet and left node is... check if 
		// left node is the required node.... if yes concat s with "0" otherwise go to the right subtree after concat "1" to s.

		if (root->right->ch == INF && root->left->ch!=INF) {
			if (root->left->p == p){
				strcat(s,"0");
				root = temp ;
				return s;
			}
			else{
				strcat(s,"1");
				root = root->right;
			}
		}
		// Repeat the above procedure and either return or go to the left subtree based on the  condition.
		
		else if (root->left->ch == INF && root->right->ch!=INF){
			if (root->right->p == p){
				strcat(s,"1");
				root = temp ;
				return s;
			}
			else{
				strcat(s,"0");
				root = root->left;
			}
		}
		
		/*If both the children are alphabets check for the values of them and 
		 * return the appropriate string*/
		
		else {
			if (root->right->ch == k){
				strcat(s,"1");
				root = temp ;
				return s;
			}
			else if (root->left->ch ==k){
				strcat(s,"0");
				root = temp;
				return s;
			}
		} 
	}
}

// Convert the text into a bit array.	

char* getBitString (char* s, char** codeTable) {

	// Allocate memory.

	char* bitString = (char*)malloc(sizeof(char)*strlen(s)*ALPHABET);
	int i ;

	// Get index of the char s[i] in codeTable and concat it to bitString.

	for (i = 0; i < strlen(s)-1 ; i++){
		strcat(bitString,codeTable[s[i]-'a']);
	}

	// Return the resultant bitString.
	return bitString ;

}
// Random integer generator to generate an int in between 0 to 10^5

int randomIntegerGenerator() {
	int r = rand()%100000;
	return r;
}

// Get the packet of specified index.

char* getPacket (char* s , int i) {
	char* msg = (char*)malloc(sizeof(char)*PACKET_SIZE);
	int start = i*PACKET_SIZE;
	int end   = (i+1)*PACKET_SIZE;
	int j ;
	int k = 0;
	for (j = start,k = 0 ; j < end && k< PACKET_SIZE ; j++){
		msg[k] = s[j] ;
		k++ ;
	}
	
	return msg ;
}

// realisation of XOR function of two chars...
char xor (char a,char b){
	if (a == b)
		return '0' ;
	else 
		return '1';
}

// Divide the message using generator polynomial... "base 2 division using XOR gate". 
char* getCheckBits (char* msg , char* generator,int index) {
	int i ;
	
	// If the generator crosses the message string stop calling...
	
	if (index >= strlen(msg) - strlen(generator)+1){
	
		// Get the remainder which is last (len)generator -1 bits .
	
		char* remainder = (char*)malloc(sizeof(char));
		for (i = index ; i < strlen(msg) ; i++){
			remainder[i-index] = msg[i]; 
		}
		return remainder ;
	}
	// Otherwise continue the division.
	// newMessage will be the updated message.
	char* newMessage = (char*)malloc(sizeof(char)*strlen(msg));
	for ( i = 0;i < index ; i++){
		newMessage[i] = msg[i];
	}
	
	// XOR the new message with generator staring from "index".
	
	for ( i = index ; i < index+strlen(generator) ; i++) {		
		newMessage[i] = xor(msg[i],generator[i-index]);
	}
	int j ;
	// Assign the remaining bits.
	for (j = i ; j < strlen(msg) ; j++){
		newMessage[j] = msg[j] ;
	}
	// Increment index by 1 and recursively call the function to carry on division.
	
	return (getCheckBits (newMessage, generator,index+1));
	 
}
	



/*----------------- MAIN FUNCTION --------------------------*/
int main() {

	FILE *fp ;
	// test.txt contains only alphabets of lower case.
	fp = fopen("test.txt","r+");
	char* s = (char*)malloc(sizeof(char)*1000000);
	// Get the input into a string s.
	fscanf(fp,"%s",s);
	// len is the string's length   
	int len = strlen(s);
	int i;
	// charCount ---- to find the frequency of each alphabet.
	int charCount[ALPHABET];
	for (i = 0; i < ALPHABET ; i++)
		charCount[i] = 0;
		
	for (i = 0; i < len ; i++){
		charCount[s[i]- 'a']+=1;	
	}
	
	// pb ----- to find the probability of occurence of each alphabet.
	double pb[ALPHABET];	
	for ( i = 0 ; i < ALPHABET ; i++){
		pb[i] = (double)charCount[i]/len;	
	}
	// a dublicate pb[] just in case....
	double pbDupe[ALPHABET];
	for (i = 0; i < ALPHABET ; i++){
		pbDupe[i] = pb[i] ;
	}
	
	// information ----- log(1/pb[i]) base 2
	double information[ALPHABET];
	for (i = 0 ; i < ALPHABET ; i++){
		if (pb[i]==0)
			information[i] = 0;
		else
			information[i] = -log(pb[i])/log(2.0) ; 
	}
	// Calculating the entropy of the source.
	double entropy = 0.0 ;
	for (i = 0 ; i < ALPHABET ; i++){
		entropy += pb[i]*information[i];
	}
	
	// Initialising the code table
	codeTable = (char**) malloc(26*sizeof(char*));
	for (i = 0 ; i < ALPHABET ; i++){
		*(codeTable+i) = (char*) malloc(sizeof(char)*100);
	}
	// Obtain the root of the Huffmann tree.
	// This has to be done by constructing the Huffmann tree.
	Node* root = (Node*) malloc(sizeof(Node));
	root = makeHuffmannTree(pb);
	
	
	// Call the getCode() function to search in the Huffmann tree to get the appropriate code of the alphabet.
	for (i = 0 ; i< 26 ; i++){
		codeTable[i] = getCode(root,pbDupe[i],i);
	}
	
	// bitString that we have to transmit.
 	char* bitString = (char*)malloc(sizeof(char)*strlen(s)*ALPHABET);
	bitString = getBitString(s,codeTable);
	
	// List of packets... Memory allocation 
	char** packet  = (char**)malloc(sizeof(char*)*((len/PACKET_SIZE)+1));
	for (i = 0 ; i < (len/PACKET_SIZE)+2 ; i++){
		packet[i] = (char*)malloc(sizeof(char)*(PACKET_SIZE*2));	
	}
	
	// Generating the packets...
	i = 0; 
	int size = 0;
	while ( i*PACKET_SIZE< len) {
		packet[i] = getPacket(bitString,i);
		size += 1 ;
	    i += 1 ;
	}
	
	// Final packets to be transmitted...
	char** finalPacket = (char**)malloc(sizeof(char*)*size);
	for (i = 0 ; i < (len/PACKET_SIZE)+2 ; i++){
		finalPacket[i] = (char*)malloc(sizeof(char)*size);	
	}
	
	/*
	char* msg = (char*)malloc(sizeof(char)*14);
	msg = "01011101010110" ;
	char* generator = (char*) malloc(sizeof(char)*5);
	generator = "11011";
	printf("%s\n",getCheckBits(msg,generator,0));
	*/
	
	char* generator = (char*) malloc(sizeof(char)*5);
	generator = "1011";
	for (i = 0 ; i < len/PACKET_SIZE ; i++){
		finalPacket[i] = packet[i];
		
		strcat(finalPacket[i],getCheckBits(packet[i],generator,0));
	
	}
	
	printf("%s\n",finalPacket[0]);
	
	return 0;
	
}
	

 
	

