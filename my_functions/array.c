int maxSubArraySum(int mat_contagions[][], int num_countrys) 
{ 
    int max_so_far = INT_MIN, max_ending_here = 0; 
  
    for (int i = 0; i < num_countrys; i++) 
    { 
        max_ending_here = max_ending_here + mat_contagios[i]; 
        if (max_so_far < max_ending_here) 
            max_so_far = max_ending_here; 
  
        if (max_ending_here < 0) 
            max_ending_here = 0; 
    } 
    return max_so_far; 
} 
