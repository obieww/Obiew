package com.obiew;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication(scanBasePackages = {"com.obiew.Controllers", "com.obiew.Entities", "com.obiew.Repositories"})
public class LoadBalanceApplication {

    public static void main(String[] args) {
        SpringApplication.run(LoadBalanceApplication.class, args);
    }

}
