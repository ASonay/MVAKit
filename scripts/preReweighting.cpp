
double rew_HF_MG5Py8(int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 1.97;}
  else if (hf_class==0 && truth_corr<=2)
    {return 0.95;}
  else if ((hf_class == 1 || (hf_class == 0 && truth_corr>2)))
    {return 1.27;}
  else
    {return 0.0;}
}

double rew_HF_PhHw(int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 2.47;}
  else if (hf_class==0 && truth_corr<=2)
    {return 0.68;}
  else if ((hf_class == 1 || (hf_class == 0 && truth_corr>2)))
    {return 1.76;}
  else
    {return 0.0;}
}

double rew_HF_nominal(int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 1.60;}
  else if (hf_class==0 && truth_corr<=2)
    {return 0.84;}
  else if ((hf_class == 1 || (hf_class == 0 && truth_corr>2)))
    {return 1.21;}
  else
    {return 0.0;}
}
