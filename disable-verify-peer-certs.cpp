int callback1(X509_STORE_CTX *ctx, void *arg){
    if(ctx!=NULL){}
    if(arg!=NULL){}
    return 1;
}

int callback(int x, X509_STORE_CTX *ctx){
    if(ctx!=NULL){}
    if(x!=1){}
    return 1;
}

// Disable certs verification
SSL_set_verify(ssl, SSL_VERIFY_NONE, 0);
SSL_set_verify_depth(ssl,0);
SSL_CTX_set_cert_verify_callback(ctx, callback1,NULL);
// SSL_VERIFY_PEER|SSL_VERIFY_CLIENT_ONCE SSL_VERIFY_NONE
SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, callback);
SSL_CTX_set_verify_depth(ctx,0);
// SSL_CTX_set_verify_depth(0);   
