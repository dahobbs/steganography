#include<stdio.h>
#include<stdlib.h>
#include<string.h> //needed for strcmp function to compare strings

/*David Hobbs 
Steg code listing
Instructions: when using the program with ./steg e file1.ppm > file2.ppm
there is no prompt for the user to input a secret message. as the file is being redirected
a prompt would ruin the format of a PPM file. */


struct PPM{
	int width;
	int height;
	int max;
	char * type;
	char * comments;
	struct PPM * next; //recursive pointer used in linked list to store comments

};
struct PPM * begin = NULL; //begin and end nodes in linked list
struct PPM * end = NULL;

struct PPM *getPPM(FILE * fd){

	/*this function reads the header of a ppm file and stores comments,
	and other attributes such as height, width, in a linked list struct */

	struct PPM * image; //instance of PPM struct that holds linked list.

	char a; //to store individual characters to be read by getc

	char comms[256];//this will hold 256 characters for

	a = getc(fd);
	if (a=='P')
		a = getc(fd);

	if (a == '3'){
		a = getc(fd); //advance variable a to get past P3

		if (a == '\r' || a == '\n'){
			a = getc(fd);

			while(a == '#') {	//loop through comments

				fscanf(fd, "%[^\n\r] ", comms); //using regex to match newline characters andcarrige returns

				image = malloc(sizeof(struct PPM)); //memory allocation


				/*this linked list code block is based on a stack overflow question about linkedlists
				and can be found here
				http://stackoverflow.com/questions/3878327/easiest-way-to-read-this-line-of-text-into-a-struct
				(stack overflow, 2010)
				*/
				image->comments = strdup (comms); //using strdup to copy string into linked list

				image->next = NULL;

				if(begin !=NULL){
					end -> next = image;
					end = image;
				}
				else{
					begin = image;
					end = image;
				}


				a = getc(fd);
			}

			ungetc(a,fd);
		}

		/*set the node to end so that type, width, height, max, info can be
		input into the struct without being repeated.*/

		image=end;

		/*write into the end node of linked list information that
		does not repeat like comments. Image type, width, height, max
		*/

		image->type = "P3";

		fscanf(fd, "%d %d %d", &image->width, &image->height, &image->max);

return image;

}

//if image fails P3 check
else{

	printf("this is not a valid PPM file\n"); //error handling in case P3 is not present at top of file
	exit(0); //stops reach the end of non void function error
}

//end of function
}

void showPPM(struct PPM * image,int * * array,int l,int m){

	/*this function will print out a correctly formmatted PPM image to
	stdout. It can be redirected to a new file.*/

	int i,k; //counters for loops

	printf("%s\n",image->type); //print P3 at top of file

	image = begin; //set linked list to read from the beginning

	while(image!=NULL){
		printf("#%s\n",image->comments); //print all available comments
		image = image->next; //move one to next node in linked list
	}
	//set linked list to end to read information stored there
	image=end;

	printf("%d %1d\n",image->width,image->height);

    printf("%d\n", image->max);

    ///print out pixel data from array.
	for(i=0;i<l;i++)
    for(k=0;k<m;k++)
       printf("%d\n",array[i][k]);

   //end of function
}

int stringLength(char s[]){

	/*this auxilliary function helps determine the length of the message to be
	encoded input by user*/
	int i;
	i =0;
   while(s[i]!='\0')
    i = i+1;
   return i;

}



int** encode(int * * array, char * message){

	//this function hides a message throughout the array
	//find length of message string
	int len = stringLength(message),i,j,k;

	srand(k);// seed k with random number generator

	j=27; // start j at 27 as it divides by three and is a bit random.

	//using pointer for asciiStore so it can be malloced for unpredictable string length
	int * asciiStore;

	asciiStore = malloc(len*sizeof(int)); // allocate memory to asciistore depending on message length

	for(i=0;i<len;i++){
		/*copy the array from message to ascii store.
		this process converts the char to its ascii equivelent
		using the (int) cast.*/
		asciiStore[i] = (int)message[i];

		array[i][j] = asciiStore[i];

		/*this random number generator gives a random number and multipies it by three to keep
		the rumber in the red pixel area. j=j+k makes sure that the random number increases
		so the message is not jumbled up by random number process*/
		k=(rand()%(7)*3);
		j = j+k;
	}

	return array;

	//free previously malloced memory to preventmemory leaks
	free(asciiStore);

}

char * decode(int * * array, int * * array2, int m, int n){
/*this function takes two arrays and compares the two for inequalities.
when inequalities are found they are placed in decode messaeg char array
and printed to stdout.*/

//allocate memory to store differences found in both arrays.
char * decodeMessage = malloc(256*sizeof(char));
	int i,j,k;
	k=0; // counter for decode message array

	//loop through both arrays to check for differences
	for(i=0;i<m;i++)
     for(j=0;j<n;j++)
       if(array2[i][j] != array[i][j]){
       		decodeMessage[k] = array2[i][j];
       		k++; //increase counter
       }

	     printf("decoded message: %s\n",decodeMessage); //print decoded message
   		 free(decodeMessage);

exit(0);
}



int main(int argc,char ** argv){
	FILE * fd; //fd = file in
	FILE * fe; //this will hold file 2
	struct PPM * picture; //store information for picutre 1
	struct PPM * picture2; //store information for picture 2
	int i,j,k,m,n; // initialise int varibales for various loop counters

	char * message;	//this char pointer will store a message form the user

	//code block that checks for the correct number of arguments
	if(argc<3){
		printf("steg:wrong number of arguments\n");
		exit(0);
	}
	fd = fopen(argv[2],"r");
	if(fd == NULL){
		printf("steg: can't open %s\n",argv[2]);
		exit(0);
	}

	//encode section when user inputs 'e' as argument
	if(strcmp(argv[1],"e") == 0){


		picture = getPPM(fd); //send file to getPPM to get info (width/height etc.) and comments


		m = picture->width;
		n = (picture ->height)*3; // make height 3 times reported height to make room for all the RBG data


		//making space for array.
		 int ** array = (int **)malloc(m*sizeof(int *));

		  		 for(i=0;i<m;i++) //for every column, make a row
		    	array[i] = (int *)malloc(n*sizeof(int));

		    //scan all integers (pixel data) into the array
		    for(j=0;j<m;j++)
		    for(k=0;k<n;k++)
		     fscanf(fd,"%d",&(array[j][k]));



		 message = malloc(256*sizeof(char)); //allocate space for 256 character message

		fgets(message,256,stdin); //this scans in user input and stores it in message
		encode(array,message); // send message and array to encode function
		showPPM(picture,array,m,n); //print out encoded array

		free(message); //free malloced memory from array
		for(i=0;i<m;i++){
				free(array[i]);
			}
		free(array);
	}




	//decode section
	if(strcmp(argv[1],"d") == 0){

		//opens the 3rd argument file for comparison to argv[2]
		fe = fopen(argv[3],"r");
		if(fe == NULL){
		printf("steg: can't open %s\n",argv[3]);
		exit(0);
		}

		picture = getPPM(fd);
		picture2 = getPPM(fe);

		m = picture->width;
		n = (picture ->height)*3; // make height 3 times reported height to make room for all the RBG data


		//making space for 2 arrays for comparison in decode function.
		 int ** array = (int **)malloc(m*sizeof(int *));

	  		for(i=0;i<m;i++)
	    	array[i] = (int *)malloc(n*sizeof(int));

		    for(j=0;j<m;j++)
		    for(k=0;k<n;k++)
		     fscanf(fd,"%d",&(array[j][k]));

		 int ** array2 = (int **)malloc(m*sizeof(int *));

  		 	for(i=0;i<m;i++)
    		array2[i] = (int *)malloc(n*sizeof(int));

    		for(j=0;j<m;j++)
    		for(k=0;k<n;k++)
     		fscanf(fe,"%d",&(array2[j][k]));

 		//send both arrays to decode along with width (m), and height(n) data.
 		decode(array, array2, m, n);

 		//free malloced memory from arrays
 		for(i=0;i<m;i++){

			free(array[i]);
			free(array2[i]);
		}

		//close open file
		fclose(fe);
		//free any extra memory from arrays 1 & 2
		free(array);
		free(array2);

	}

//close open file
fclose(fd);
}
