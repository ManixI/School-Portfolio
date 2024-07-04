####CCA Demonstration
###Import PYD_Barbados dataset
data=na.omit(PYD_Barbados[10:19])
#define canonical sets
PYD=data[,1:5]
CDSE=data[,6:10]

###CCA
install.packages("CCA")
install.packages("CCP")
library(CCA)
cca1=cc(PYD,CDSE)
##do i have something?
#canonical correlations
cca1$cor

#dimension reduction analysis
library(CCP)
rho=cca1$cor
n=dim(PYD)[1]
p=length(PYD)
q=length(CDSE)
p.asym(rho,n,p,q,tstat="Wilks")

#Only full canonical functions significant.. has Rc of .42, so only interpret first canonical function

##If I have something, where does it come from?
#unstandardized coefficients
cca1[3:4]
#to get standardized coefficients see https://stats.oarc.ucla.edu/r/dae/canonical-correlation-analysis/

#structure coefficients - "loadings"
cca2=comput(PYD,CDSE,cca1)
cca2[c("corr.X.xscores","corr.Y.yscores")]

