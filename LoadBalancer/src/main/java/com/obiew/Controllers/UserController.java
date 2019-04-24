package com.obiew.Controllers;

import com.obiew.Entities.User;
import com.obiew.Repositories.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api")
public class UserController {
    @Autowired
    private UserRepository userRepository;
    @PostMapping("/register")
    public ResponseEntity<User> register(@RequestBody User user) {
        if(userRepository.findByUsername(user.getUsername()) != null) {
            return new ResponseEntity<>(null, HttpStatus.FORBIDDEN);
        }
        userRepository.save(user);
        return new ResponseEntity<>(user, HttpStatus.OK);
    }
}
