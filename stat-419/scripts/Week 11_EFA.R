####EFA
install.packages("psych")
library(psych)
#import Holzinger data_td
#rename data
efaData=data[,5:13]
#create correlation matrix
cordata=cor(efaData,use="pairwise.complete.obs")
#parallel analysis and scree plot
fa.parallel(x=cordata,n.obs=301,fm="minres",fa="fa")
#keep 3 factors
install.packages("EFA.dimensions")
library(EFA.dimensions)
MAP(cordata,Ncases=301)
#suggests keeping 2
#going with 3 since scree and parallel suggest 3
fa1=fa(r=cordata,n.obs=301,nfactors=3,rotate="promax",fm="pa")
fa1

#factor correlations - decide if switch to orthogonal
fa1$Phi
#one correlation >.32, so keep oblique

#item decisions
fa1$Structure
fa1$loadings
fa1$communality

#coefficient alpha reliabilities
install.packages("ltm")
library(ltm)
cronbach.alpha(efaData[,1:3], CI=TRUE)
cronbach.alpha(efaData[,4:6], CI=TRUE)
cronbach.alpha(efaData[,7:9], CI=TRUE)
