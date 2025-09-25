%Quantalph code from SRD resource files
function y=quantalph(x,alphabet)
alphabet=alphabet(:);
x=x(:);
alpha=alphabet(:,ones(size(x)))';
dist=(x(:,ones(size(alphabet)))-alpha).^2;
[v,i]=min(dist,[],2);
y=alphabet(i);