FROM ubuntu:20.04

ARG DB_HOST
ARG DB_PORT
ARG DB_NAME
ARG DB_USER
ARG DB_PASS

ENV DB_HOST=$DB_HOST
ENV DB_PORT=$DB_PORT
ENV DB_NAME=$DB_NAME
ENV DB_USER=$DB_USER
ENV DB_PASS=$DB_PASS

# Set the timezone
ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Update and install necessary packages
RUN apt update
RUN apt install -y redis-server git
RUN apt install -y cmake g++ gcc libjsoncpp-dev uuid-dev openssl
RUN apt install -y libssl-dev zlib1g-dev libbz2-dev liblzma-dev
RUN apt install -y postgresql postgresql-contrib
RUN apt clean && rm -rf /var/lib/apt/lists/*

# Clone the Drogon repository
RUN git clone https://github.com/drogonframework/drogon

# Build and install the Drogon library
RUN cd drogon && \
    git submodule update --init && \
    mkdir build && cd build && cmake .. && make && make install

# Clone JWT-CPP repository
RUN git clone https://github.com/Thalhammer/jwt-cpp.git

# Build and install the JWT-CPP library
RUN cd jwt-cpp && mkdir build && cd build && cmake .. && make && make install

# Copy the application code
WORKDIR /app
COPY . /app

# Run scripts
RUN chmod +x scripts/create_dot_env.sh

# Build app
RUN mkdir build && cd build && cmake .. && make && chmod +x drogon_user_service

# Expose port 8000 for the app
EXPOSE 8000

# Start the app
CMD ["/app/build/drogon_user_service", "--action=run-server"]
