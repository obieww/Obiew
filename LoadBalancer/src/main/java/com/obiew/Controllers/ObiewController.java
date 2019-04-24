package com.obiew.Controllers;

import com.obiew.Entities.Obiew;
import com.obiew.Repositories.ObiewRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api/obiew")
public class ObiewController {
    @Autowired
    private ObiewRepository obiewRepository;
    @GetMapping("/{obiewId}")
    public ResponseEntity<Obiew> getObiewById(@PathVariable String obiewId) {
        Obiew obiew =
    }
}
