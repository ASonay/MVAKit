
double rew_HF_MG5Py8(int nlep, int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 1.77;}
  else if (hf_class==0 && truth_corr<=2)
    {
      if (nlep==1)
	{return 0.94;}
      else if (nlep==2)
	{return 0.96;}
      else
	{return 0.0;}
    }
  else if ((hf_class == 1 || (hf_class == 0 && truth_corr>2)))
    {return 1.30;}
  else
    {return 0.0;}
}

double rew_HF_PhHw(int nlep, int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 2.21;}
  else if (hf_class==0 && truth_corr<=2)
    {
      if (nlep==1)
	{return 0.67;}
      else if (nlep==2)
	{return 0.73;}
      else
	{return 0.0;}
    }
  else if ((hf_class == 1 || (hf_class == 0 && truth_corr>2)))
    {return 1.59;}
  else
    {return 0.0;}
}

double rew_HF_nominal(int nlep, int hf_class, int truth_corr, int is4fs=0) {
  if (is4fs) {
    if (hf_class==-1)
      {return 1.70;}
    else if (hf_class==0 && truth_corr<=2)
      {
	if (nlep==1)
	  {return 0.83;}
	else if (nlep==2)
	  {return 0.86;}
	else
	  {return 0.0;}
      }
    else if ((hf_class == 1 || (hf_class == 0 && truth_corr>2)))
      {return 1.14;}
    else
      {return 0.0;}
  }
  else {
    if (hf_class==-1)
      {return 1.61;}
    else if (hf_class==0 && truth_corr<=2)
      {
	if (nlep==1)
	  {return 0.84;}
	else if (nlep==2)
	  {return 0.87;}
	else
	  {return 0.0;}
      }
    else if ((hf_class == 1 || (hf_class == 0 && truth_corr>2)))
      {return 1.21;}
    else
      {return 0.0;}
  }
}
